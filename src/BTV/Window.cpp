#include "Window.h"
#include "resource.h"

using namespace BTV;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static Controller *ctrl = NULL;
	if (Message == WM_NCCREATE) {      // 窗口创建前会收到此消息
		if(!ctrl) ctrl = (Controller *)(((CREATESTRUCT*)lParam)->lpCreateParams);  // 保存主窗口控制器
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	if (!ctrl) return DefWindowProc(hWnd, Message, wParam, lParam);  // 控制器不可用前不处理消息
	switch (Message) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MENU_OPEN:   // 菜单或加速键打开文件
			ctrl->open();
			break;
		case MENU_SAVE:   // 菜单或加速键保存文件
			ctrl->save();
			break;
		case MENU_EXIT:   // 菜单或加速键退出程序
			PostQuitMessage(0);
			break;
		case MENU_HELP:   // 菜单或加速键显示帮助
			MessageBox(hWnd, TEXT(
				"文件操作：\n"
				"打开：打开树操作脚本或二进制树形文件\n"
				"保存：保存当前树形为二进制文件\n\n"
				"功能：\n"
				"开始：开始动画演示\n"
				"暂停：暂停动画演示\n"
				"单步：单步演示 B 树动画\n"
				"停止：跳过动画，显示 B 树最终形态\n"
				"速度：右下角滑块可调\n\n"
				"快捷键：\n"
				"开始动画：F9\n"
				"暂停动画：F10\n"
				"单步动画：F11\n"
				"结束动画：F12\n"
				"打开文件：Ctrl+O\n"
				"保存文件：Ctrl+S\n"
				"获取帮助：Ctrl+H\n"
				"查看关于：Ctrl+I\n"
				"退出程序：Ctrl+Q\n\n"
				"Tips：可直接拖拽文件到窗口来打开文件"
			), TEXT("帮助"), MB_OK);
			break;
		case MENU_ABOUT:  // 菜单或加速键显示关于页面
			MessageBox(hWnd, TEXT(
				"集美大学\n"
				"计算机工程学院\n"
				"软件1813 夏张\n"
				"Copyright 2019\n\n"
				"指导老师：张庆"
			), TEXT("关于"), MB_OK);
			break;
		case MENU_START:  // 菜单或加速键开始动画
			ctrl->animation(ANIMATION_START);
			break;
		case MENU_PAUSE:  // 菜单或加速键显示暂停动画
			ctrl->animation(ANIMATION_PAUSE);
			break;
		case MENU_STEP:   // 菜单或加速键单步演示动画
			ctrl->step();
			break;
		case MENU_STOP:   // 菜单或加速键显示结束动画
			ctrl->animation(ANIMATION_STOP);
			break;
		}
		break;
	case WM_SIZE:   // 窗口大小发生改变
		ctrl->resize(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		if (ctrl->paint(hWnd)) return DefWindowProc(hWnd, Message, wParam, lParam);  // 如果是主窗口就自动重绘
		else break;  // 如果是画布窗口就我们自己重绘，一方面增加流畅度、减少不必要资源消耗，另一方面可防止画面暂停时窗口拖到屏幕外画布被清空的问题
	case WM_DESTROY:          // 窗口被摧毁
		PostQuitMessage(0);   // 发送退出消息
		break;
	case WM_GETMINMAXINFO: {
		MINMAXINFO *mminfo = (PMINMAXINFO)lParam;
		mminfo->ptMinTrackSize.x = 1150;  // 设置最小窗口宽度
		mminfo->ptMinTrackSize.y = 300;   // 设置最小窗口高度
		break;
	}
	case WM_DROPFILES:
		ctrl->drag((HDROP)wParam);
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);  // 默认处理消息函数
	}
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static Controller *ctrl = NULL;
	if (Message == WM_INITDIALOG){      // 对话窗口创建前会受到此消息
		ctrl = (Controller*)lParam;     // 保存控制窗口控制器
		return true;
	}
	if (!ctrl) return false;            // 控制器不可用前不处理消息
	switch (Message) {
	case WM_COMMAND:                    // 接收按钮消息
		ctrl->command(LOWORD(wParam));
		break;
	case WM_HSCROLL:                    // 接收滑块消息
		ctrl->speed();
		break;
	default:                            // 其他消息不处理
		return false;
	}
	return true;
}

//下面使用初始化列表来初始化一些变量，提高程序运行效率；窗口大小位置为默认值
Window::Window(HINSTANCE hInst, LPCWSTR name, HWND hParent, Controller *ctrl) : handle(NULL), instance(hInst),
                                                                                winStyle(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN),
                                                                                winStyleEx(WS_EX_CLIENTEDGE), x(CW_USEDEFAULT), y(CW_USEDEFAULT),
                                                                                width(CW_USEDEFAULT), height(CW_USEDEFAULT), ctrl(ctrl),
                                                                                parentHandle(hParent), menuHandle(NULL) {

	title = className = name;                                           // 设置窗口名字和类名

	// WNDCLASSEX 参数设置
	memset(&winClass, 0, sizeof(winClass));                             // 结构体填零

	winClass.cbSize        = sizeof(WNDCLASSEX);                        // WNDCLASSEX 结构体大小
	winClass.lpfnWndProc   = WndProc;                                   // 窗口回调函数指针
	winClass.hInstance     = instance;                                  // 窗口实例
	winClass.hIcon         = LoadIcon(instance, IDI_APPLICATION);       // 默认图标
	winClass.hCursor       = LoadCursor(0, IDC_ARROW);                  // 默认指针
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);       // 默认黑色背景
	winClass.lpszClassName = className;                                 // 类名
	winClass.hIconSm       = LoadIcon(instance, IDI_APPLICATION);       // 默认小图标
}

Window::~Window(void) {
	UnregisterClass(className, instance);         // 取消类注册
}

HWND Window::create(void) {
	if (!RegisterClassEx(&winClass)) return NULL;    // 注册类

	handle = CreateWindowEx(
		winStyleEx,           // 扩展样式
		className,            // 类名
		title,                // 窗口标题
		winStyle,             // 窗口样式
		x,                    // 窗口水平位置
		y,                    // 窗口竖直位置
		width,                // 窗口宽度
		height,               // 窗口高度
		parentHandle,         // 父窗口句柄
		menuHandle,           // 菜单句柄
		instance,             // 程序实例
		(LPVOID)ctrl          // 窗口数据
	);

	return handle;
}

void Window::show(int cmdShow) {
	ShowWindow(handle, cmdShow);     // 设置显示窗口
	UpdateWindow(handle);            // 更新窗口
}

HICON Window::loadIcon(int id) {
	return (HICON)LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);      // 返回程序图标资源
}

HCURSOR Window::loadCursor(int id) {
	return (HCURSOR)LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);  // 返回指针图标资源
}