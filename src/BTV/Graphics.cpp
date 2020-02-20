#include "Graphics.h"
#include <commctrl.h>
#include <chrono>

using namespace BTV;

Canvas::Canvas(HWND hWnd) : hWnd(hWnd), loop(true), resized(false), waitTime(SLEEP_TIME), status(true), block(true), lastTime(-1.0f) {   // 初始化变量
	renderThread = std::thread(&Canvas::render, this);  // 创建渲染线程
}

Canvas::~Canvas() {
	loop = false;          // 关闭循环渲染
	block = false;         // 取消动画阻塞
	cv.notify_one();       // 取消动画等待
	push(UNBLOCK_QUEUE);   // 让线程取消阻塞
	renderThread.join();   // 等待线程退出
}

void Canvas::init() {
	PIXELFORMATDESCRIPTOR pfd;                   // 像素格式描述符
	int iFormat;                                 // 像素格式

	ZeroMemory(&pfd, sizeof(pfd));               // 结构体填零

	pfd.nSize      = sizeof(pfd);                // 设置结构体大小
	pfd.nVersion   = 1;                          // 设置版本号
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW |        // 绘制到窗口
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;   // 支持 OpenGL, 开启双重缓冲（防止闪屏）
	pfd.iPixelType = PFD_TYPE_RGBA;              // RGBA 颜色模式
	pfd.cColorBits = 24;                         // 设置颜色位数
	pfd.cDepthBits = 16;                         // 设置深度缓冲区表示一个像素所用位数
	pfd.iLayerType = PFD_MAIN_PLANE;             // 设置层类型

	hDC = GetDC(hWnd);                           // 获取设备上下文
	iFormat = ChoosePixelFormat(hDC, &pfd);      // 选择合适像素格式
	SetPixelFormat(hDC, iFormat, &pfd);          // 设置像素格式
	hRC = wglCreateContext(hDC);                 // 创建渲染上下文
	wglMakeCurrent(hDC, hRC);                    // 设置渲染到该上下文

	font = glGenLists(MAX_CHAR);                 // 创建显示列表
	HFONT hFont = CreateFont(
		1, 1,                        // 字体高宽
		0, 0,                        // 字体角度
		FW_NORMAL,                   // 正常字体粗细
		FALSE, FALSE, FALSE,         // 不使用斜体，下划线和删除线
		ANSI_CHARSET,                // 字符集类型
		OUT_TT_PRECIS,               // 使用 TrueType 字体
		CLIP_DEFAULT_PRECIS,         // 默认剪裁精度
		ANTIALIASED_QUALITY,         // 字体质量
		FF_DONTCARE | DEFAULT_PITCH, // 字体间距和字体族
		TEXT("")                     // 字体
	);
	SelectObject(hDC, hFont);  // 使用该字体
	wglUseFontOutlines(
		hDC,       // 当前窗口设备上下文
		0,         // 用于创建显示列表字体的第一个字符的ASCII值
		MAX_CHAR,  // 字符数
		font,      // 第一个显示列表的名称
		0.0f,      // 字体的光滑度，越小越光滑
		0.0f,      // 在 Z 方向突出的距离
		WGL_FONT_POLYGONS,   // 使用多边形来生成字符
		gmf        // 一个接收字形度量数据的数组的地址，每个数组元素用它对应的显示列表字符的数据填充
	);
	glListBase(font);     // 设置显示列表的基础值
	push(CANVAS_CLEAN);   // 清空画布内容
}

void Canvas::render() {
	init();               // 初始化
	Queue::Node *node;    // 保存队列节点
	while (loop) {
		node = que.pop();     // 获取操作，无操作时阻塞
		if (resized) {        // 如果窗口大小改变
			handleResize();   // 处理窗口改变事件
			continue;         // 跳过本次事件路由
		}
		switch (node->type) { // 事件路由
		case CANVAS_INIT:     // 初始化画布
			left = node->x1;
			right = node->x2;
			bottom = node->y1;
			top = node->y2;
			glClear(GL_COLOR_BUFFER_BIT); // 清除画布颜色
			que.destory(node);            // 销毁之前队列元素
			handleResize();               // 设置画布大小
			if (node->value && lastTime >= 0.0f) {
				bar->msg(TIME_STATUSBAR, L"上次操作用时：%f 毫秒", lastTime);
				node->value = 0;          // 重绘时不重复设置状态栏
			}
			break;
		case DRAW_NODE:  // 画节点
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // 绘制线框模式
			glBegin(GL_QUAD_STRIP);                     // 绘制矩形
			glColor3f(1.0f, node->x2, node->x2);        // 设置矩形颜色
			for (int i = 0; i < node->value; i++) {     // 绘制矩形顶点
				glVertex2f(node->x1 + i, node->y1 + 1.0f);
				glVertex2f(node->x1 + i, node->y1);
			}
			glEnd();
			break;
		case DRAW_LINE:          // 画指针连线
			glBegin(GL_LINES);   // 绘制直线
			glColor3f(1.0f, 1.0f, 1.0f);     // 直线颜色为白色
			glVertex2f(node->x1, node->y1);  // 输入顶点
			glVertex2f(node->x2, node->y2);
			glEnd();
			break;
		case DRAW_FONT: {  // 绘制字符
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);         // 填充模式
			sprintf_s(str, MAX_STR_LEN, "%d", node->value);    // 数字转换为文字
			glColor3f(1.0f, node->x2, node->x2);               // 设置文字颜色
			glPushMatrix();                                    // 保存原先位置状态
			float width = 0.0f, height = 0.0f;
			unsigned int len = strlen(str);
			for (unsigned int i = 0; i < len; i++) {
				width += gmf[str[i]].gmfCellIncX;              // 计算文本总宽度
				if (gmf[str[i]].gmfptGlyphOrigin.y > height)   // 记录最大高度
					height = gmf[str[i]].gmfptGlyphOrigin.y;
			}
			float temp = 1.0f - NODE_PADDING * 2.0f, scale;    // 缩放高度/宽度的分子
			if (width > height) {        // 如果宽度较大
				scale = temp / width;    // 按宽度等比例缩放
				glTranslatef(node->x1 + NODE_PADDING, node->y1 + 0.5f - (height / 2.0f) * scale, 0.0f); // 选择文字位置
				glScalef(scale, scale, 1.0f);
			} else {                     // 如果高度较大
				scale = temp / height;   // 按高度等比例缩放
				glTranslatef(node->x1 + 0.5f - (width / 2.0f) * scale, node->y1 + NODE_PADDING, 0.0f);  // 选择文字位置
				glScalef(scale, scale, 1.0f);
			}
			glCallLists(len, GL_UNSIGNED_BYTE, str);   // 显示文字
			glPopMatrix();  // 恢复原先位置状态
			break;
		}
		case CANVAS_CLEAN:  // 清空画布
			que.destory(node);              // 销毁队列之前的元素
			glClear(GL_COLOR_BUFFER_BIT);   // 清除画布颜色
			SwapBuffers(hDC);               // 输出空图像
			if (node->value) {              // 第一次清屏时清除状态栏信息
				node->value = 0;
				initBar();
			}
			break;
		case CANVAS_DONE: {   // 绘制完成，渲染图像
			SwapBuffers(hDC); // 显示图像
			block = true;     // 恢复阻塞
			if (node->value) {
				lastTime = node->x1;  // 记录时间
				bar->msg(TIME_STATUSBAR, L"本次操作用时：%f 毫秒", lastTime);  // 显示用时
				node->value = 0;      // 重绘时不重复设置状态栏
			}
			start = std::chrono::steady_clock::now();       // 获取开始时间
			duration = std::chrono::microseconds::zero();   // 清零时间差
			std::unique_lock <std::mutex> lck(mtx);         // 创建锁
			while (block && duration.count() < waitTime) {  // 如果没到达指定时间就解锁，则继续等待
				cv.wait_for(lck, std::chrono::microseconds(waitTime));  // 等待一段时间
				duration = std::chrono::steady_clock::now() - start;    // 计算时间差
			}
			while (block && !status) cv.wait(lck);     // 如果画面是暂停状态，等待状态改变
			// 以上两个循环在取消渲染、窗口大小改变以及动画暂停切换的情况下不继续进行，而响应这些高优先级事件
			break;
		}
		case STATUS_INSERT:
			bar->msg(ACTION_STATUSBAR, L"插入：%d", node->value);
			break;
		case STATUS_SEARCH:
			bar->msg(ACTION_STATUSBAR, L"搜索：%d", node->value);
			break;
		case STATUS_DELETE:
			bar->msg(ACTION_STATUSBAR, L"删除：%d", node->value);
			break;
		case STATUS_PRINT:
			bar->msg(ACTION_STATUSBAR, node->value ? L"前序遍历" : L"后序遍历");
			break;
		case STATUS_FORK:
			bar->msg(ACTION_STATUSBAR, L"键值数量达到 %d 个，分裂", node->value);
			break;
		case STATUS_MERGE:
			bar->msg(ACTION_STATUSBAR, L"键值数量小于 %d 个，合并", node->value);
			break;
		case STATUS_BORROW:
			bar->msg(ACTION_STATUSBAR, node->value ? L"成功向右兄弟借到值" : L"成功向左兄弟借到值");
			break;
		case STATUS_RSEARCH:
			bar->msg(ACTION_STATUSBAR, node->value ? L"成功找到键值" : L"未找到该键值");
			break;
		case STATUS_LOAD:
			bar->msg(ACTION_STATUSBAR, node->value ? L"成功打开文件" : L"打开文件失败");
			break;
		case STATUS_SAVE:
			bar->msg(ACTION_STATUSBAR, node->value ? L"保存文件成功" : L"无法保存文件");
			break;
		}
	}
	glDeleteLists(font, MAX_CHAR);  // 释放字符资源
	wglMakeCurrent(NULL, NULL);     // 取消 OpenGL 注册
	wglDeleteContext(hRC);          // 删除上下文
	ReleaseDC(hWnd, hDC);           // 释放设备描述符
}

void Canvas::handleResize(void) {
	if (resized) {       // 如果是窗口大小改变触发的
		que.rewind();    // 设置画布重新渲染
		resized = false;
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);   // 设置视口
		aspectRatio = (GLdouble)width / (GLdouble)height;    // 计算窗口的纵横比
	}
	// 选择正交视图矩阵，并重置坐标系统
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// 根据窗口的纵横比定义裁剪区域，并使用正投影
	GLdouble w = right - left + CANVAS_PADDING * 2.0, h = top - bottom + CANVAS_PADDING * 2.0, temp;  // B 树图形长宽
	if (width / w < height / h) {  // 图形自适应窗口大小
		// 图形宽度作为最大值
		temp = w / aspectRatio / 2.0;
		glOrtho(left - CANVAS_PADDING, right + CANVAS_PADDING, -temp + h / 2.0 - CANVAS_PADDING, temp + h / 2.0 - CANVAS_PADDING, 1.0, -1.0);
	} else {
		// 图形高度作为最大值
		temp = h * aspectRatio / 2.0;
		glOrtho(-temp + w / 2.0 - CANVAS_PADDING, temp + w / 2.0 - CANVAS_PADDING, bottom - CANVAS_PADDING, top + CANVAS_PADDING, 1.0, -1.0);
	}
	// 选择模型视图矩阵，并重置坐标系统
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Canvas::unblockOnce(bool force) {
	if (!status || force) {
		block = false;       // 取消动画阻塞
		cv.notify_one();     // 解除动画等待
	}
}

void Canvas::resize(int w, int h) {
	if(w)
	width = w, height = h;   // 设置窗口高宽
	resized = true;          // 设置窗口大小改变标志位
	push(UNBLOCK_QUEUE);     // 取消队列阻塞
	unblockOnce(true);       // 取消阻塞，先重置画布大小
}

void Canvas::push(int type, int value, float x1, float y1, float x2, float y2) {
	que.push(type, x1, y1, x2, y2, value);  // 事件入队
}

void Canvas::setWaitTime(int time) {
	if (time) {
		waitTime = MAX_SLEEP_TIME + MIN_SLEEP_TIME + 1 - time;  // 设置暂停时间（滑块方向与时间大小反过来的，所以用减的）
		cv.notify_one();     // 解除动画等待，响应动作
	} else {
		que.end();           // 如果时间为零，转到最后一帧
		block = false;       // 取消动画等待
		switchStatus(true);  // 设为运行模式
	}
}

void Canvas::switchStatus(bool type) {
	if (type && !status) { // 加个判断，防止不必要的资源消耗
		status = type;     // 设置动画状态
		cv.notify_one();   // 如果是运行状态，取消暂停
		bar->msg(STATUS_STATUSBAR, L"动画状态：运行");
	} else if(!type && status) {
		status = type;     // 设置动画状态
		bar->msg(STATUS_STATUSBAR, L"动画状态：暂停");
	}
}

void Canvas::setStatusBar(StatusBar *bar) {
	this->bar = bar;  // 设置状态栏句柄
	initBar();        // 初始化文字
}

void Canvas::initBar(void) {
	bar->msg(ACTION_STATUSBAR, L"就绪");
	bar->msg(STATUS_STATUSBAR, L"动画状态：运行");
}