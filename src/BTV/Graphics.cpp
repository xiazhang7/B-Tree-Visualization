#include "Graphics.h"
#include <commctrl.h>
#include <chrono>

using namespace BTV;

Canvas::Canvas(HWND hWnd) : hWnd(hWnd), loop(true), resized(false), waitTime(SLEEP_TIME), status(true), block(true), lastTime(-1.0f) {   // ��ʼ������
	renderThread = std::thread(&Canvas::render, this);  // ������Ⱦ�߳�
}

Canvas::~Canvas() {
	loop = false;          // �ر�ѭ����Ⱦ
	block = false;         // ȡ����������
	cv.notify_one();       // ȡ�������ȴ�
	push(UNBLOCK_QUEUE);   // ���߳�ȡ������
	renderThread.join();   // �ȴ��߳��˳�
}

void Canvas::init() {
	PIXELFORMATDESCRIPTOR pfd;                   // ���ظ�ʽ������
	int iFormat;                                 // ���ظ�ʽ

	ZeroMemory(&pfd, sizeof(pfd));               // �ṹ������

	pfd.nSize      = sizeof(pfd);                // ���ýṹ���С
	pfd.nVersion   = 1;                          // ���ð汾��
	pfd.dwFlags    = PFD_DRAW_TO_WINDOW |        // ���Ƶ�����
		PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;   // ֧�� OpenGL, ����˫�ػ��壨��ֹ������
	pfd.iPixelType = PFD_TYPE_RGBA;              // RGBA ��ɫģʽ
	pfd.cColorBits = 24;                         // ������ɫλ��
	pfd.cDepthBits = 16;                         // ������Ȼ�������ʾһ����������λ��
	pfd.iLayerType = PFD_MAIN_PLANE;             // ���ò�����

	hDC = GetDC(hWnd);                           // ��ȡ�豸������
	iFormat = ChoosePixelFormat(hDC, &pfd);      // ѡ��������ظ�ʽ
	SetPixelFormat(hDC, iFormat, &pfd);          // �������ظ�ʽ
	hRC = wglCreateContext(hDC);                 // ������Ⱦ������
	wglMakeCurrent(hDC, hRC);                    // ������Ⱦ����������

	font = glGenLists(MAX_CHAR);                 // ������ʾ�б�
	HFONT hFont = CreateFont(
		1, 1,                        // ����߿�
		0, 0,                        // ����Ƕ�
		FW_NORMAL,                   // ���������ϸ
		FALSE, FALSE, FALSE,         // ��ʹ��б�壬�»��ߺ�ɾ����
		ANSI_CHARSET,                // �ַ�������
		OUT_TT_PRECIS,               // ʹ�� TrueType ����
		CLIP_DEFAULT_PRECIS,         // Ĭ�ϼ��þ���
		ANTIALIASED_QUALITY,         // ��������
		FF_DONTCARE | DEFAULT_PITCH, // �������������
		TEXT("")                     // ����
	);
	SelectObject(hDC, hFont);  // ʹ�ø�����
	wglUseFontOutlines(
		hDC,       // ��ǰ�����豸������
		0,         // ���ڴ�����ʾ�б�����ĵ�һ���ַ���ASCIIֵ
		MAX_CHAR,  // �ַ���
		font,      // ��һ����ʾ�б������
		0.0f,      // ����Ĺ⻬�ȣ�ԽСԽ�⻬
		0.0f,      // �� Z ����ͻ���ľ���
		WGL_FONT_POLYGONS,   // ʹ�ö�����������ַ�
		gmf        // һ���������ζ������ݵ�����ĵ�ַ��ÿ������Ԫ��������Ӧ����ʾ�б��ַ����������
	);
	glListBase(font);     // ������ʾ�б�Ļ���ֵ
	push(CANVAS_CLEAN);   // ��ջ�������
}

void Canvas::render() {
	init();               // ��ʼ��
	Queue::Node *node;    // ������нڵ�
	while (loop) {
		node = que.pop();     // ��ȡ�������޲���ʱ����
		if (resized) {        // ������ڴ�С�ı�
			handleResize();   // �����ڸı��¼�
			continue;         // ���������¼�·��
		}
		switch (node->type) { // �¼�·��
		case CANVAS_INIT:     // ��ʼ������
			left = node->x1;
			right = node->x2;
			bottom = node->y1;
			top = node->y2;
			glClear(GL_COLOR_BUFFER_BIT); // ���������ɫ
			que.destory(node);            // ����֮ǰ����Ԫ��
			handleResize();               // ���û�����С
			if (node->value && lastTime >= 0.0f) {
				bar->msg(TIME_STATUSBAR, L"�ϴβ�����ʱ��%f ����", lastTime);
				node->value = 0;          // �ػ�ʱ���ظ�����״̬��
			}
			break;
		case DRAW_NODE:  // ���ڵ�
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);  // �����߿�ģʽ
			glBegin(GL_QUAD_STRIP);                     // ���ƾ���
			glColor3f(1.0f, node->x2, node->x2);        // ���þ�����ɫ
			for (int i = 0; i < node->value; i++) {     // ���ƾ��ζ���
				glVertex2f(node->x1 + i, node->y1 + 1.0f);
				glVertex2f(node->x1 + i, node->y1);
			}
			glEnd();
			break;
		case DRAW_LINE:          // ��ָ������
			glBegin(GL_LINES);   // ����ֱ��
			glColor3f(1.0f, 1.0f, 1.0f);     // ֱ����ɫΪ��ɫ
			glVertex2f(node->x1, node->y1);  // ���붥��
			glVertex2f(node->x2, node->y2);
			glEnd();
			break;
		case DRAW_FONT: {  // �����ַ�
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);         // ���ģʽ
			sprintf_s(str, MAX_STR_LEN, "%d", node->value);    // ����ת��Ϊ����
			glColor3f(1.0f, node->x2, node->x2);               // ����������ɫ
			glPushMatrix();                                    // ����ԭ��λ��״̬
			float width = 0.0f, height = 0.0f;
			unsigned int len = strlen(str);
			for (unsigned int i = 0; i < len; i++) {
				width += gmf[str[i]].gmfCellIncX;              // �����ı��ܿ��
				if (gmf[str[i]].gmfptGlyphOrigin.y > height)   // ��¼���߶�
					height = gmf[str[i]].gmfptGlyphOrigin.y;
			}
			float temp = 1.0f - NODE_PADDING * 2.0f, scale;    // ���Ÿ߶�/��ȵķ���
			if (width > height) {        // �����Ƚϴ�
				scale = temp / width;    // ����ȵȱ�������
				glTranslatef(node->x1 + NODE_PADDING, node->y1 + 0.5f - (height / 2.0f) * scale, 0.0f); // ѡ������λ��
				glScalef(scale, scale, 1.0f);
			} else {                     // ����߶Ƚϴ�
				scale = temp / height;   // ���߶ȵȱ�������
				glTranslatef(node->x1 + 0.5f - (width / 2.0f) * scale, node->y1 + NODE_PADDING, 0.0f);  // ѡ������λ��
				glScalef(scale, scale, 1.0f);
			}
			glCallLists(len, GL_UNSIGNED_BYTE, str);   // ��ʾ����
			glPopMatrix();  // �ָ�ԭ��λ��״̬
			break;
		}
		case CANVAS_CLEAN:  // ��ջ���
			que.destory(node);              // ���ٶ���֮ǰ��Ԫ��
			glClear(GL_COLOR_BUFFER_BIT);   // ���������ɫ
			SwapBuffers(hDC);               // �����ͼ��
			if (node->value) {              // ��һ������ʱ���״̬����Ϣ
				node->value = 0;
				initBar();
			}
			break;
		case CANVAS_DONE: {   // ������ɣ���Ⱦͼ��
			SwapBuffers(hDC); // ��ʾͼ��
			block = true;     // �ָ�����
			if (node->value) {
				lastTime = node->x1;  // ��¼ʱ��
				bar->msg(TIME_STATUSBAR, L"���β�����ʱ��%f ����", lastTime);  // ��ʾ��ʱ
				node->value = 0;      // �ػ�ʱ���ظ�����״̬��
			}
			start = std::chrono::steady_clock::now();       // ��ȡ��ʼʱ��
			duration = std::chrono::microseconds::zero();   // ����ʱ���
			std::unique_lock <std::mutex> lck(mtx);         // ������
			while (block && duration.count() < waitTime) {  // ���û����ָ��ʱ��ͽ�����������ȴ�
				cv.wait_for(lck, std::chrono::microseconds(waitTime));  // �ȴ�һ��ʱ��
				duration = std::chrono::steady_clock::now() - start;    // ����ʱ���
			}
			while (block && !status) cv.wait(lck);     // �����������ͣ״̬���ȴ�״̬�ı�
			// ��������ѭ����ȡ����Ⱦ�����ڴ�С�ı��Լ�������ͣ�л�������²��������У�����Ӧ��Щ�����ȼ��¼�
			break;
		}
		case STATUS_INSERT:
			bar->msg(ACTION_STATUSBAR, L"���룺%d", node->value);
			break;
		case STATUS_SEARCH:
			bar->msg(ACTION_STATUSBAR, L"������%d", node->value);
			break;
		case STATUS_DELETE:
			bar->msg(ACTION_STATUSBAR, L"ɾ����%d", node->value);
			break;
		case STATUS_PRINT:
			bar->msg(ACTION_STATUSBAR, node->value ? L"ǰ�����" : L"�������");
			break;
		case STATUS_FORK:
			bar->msg(ACTION_STATUSBAR, L"��ֵ�����ﵽ %d ��������", node->value);
			break;
		case STATUS_MERGE:
			bar->msg(ACTION_STATUSBAR, L"��ֵ����С�� %d �����ϲ�", node->value);
			break;
		case STATUS_BORROW:
			bar->msg(ACTION_STATUSBAR, node->value ? L"�ɹ������ֵܽ赽ֵ" : L"�ɹ������ֵܽ赽ֵ");
			break;
		case STATUS_RSEARCH:
			bar->msg(ACTION_STATUSBAR, node->value ? L"�ɹ��ҵ���ֵ" : L"δ�ҵ��ü�ֵ");
			break;
		case STATUS_LOAD:
			bar->msg(ACTION_STATUSBAR, node->value ? L"�ɹ����ļ�" : L"���ļ�ʧ��");
			break;
		case STATUS_SAVE:
			bar->msg(ACTION_STATUSBAR, node->value ? L"�����ļ��ɹ�" : L"�޷������ļ�");
			break;
		}
	}
	glDeleteLists(font, MAX_CHAR);  // �ͷ��ַ���Դ
	wglMakeCurrent(NULL, NULL);     // ȡ�� OpenGL ע��
	wglDeleteContext(hRC);          // ɾ��������
	ReleaseDC(hWnd, hDC);           // �ͷ��豸������
}

void Canvas::handleResize(void) {
	if (resized) {       // ����Ǵ��ڴ�С�ı䴥����
		que.rewind();    // ���û���������Ⱦ
		resized = false;
		glViewport(0, 0, (GLsizei)width, (GLsizei)height);   // �����ӿ�
		aspectRatio = (GLdouble)width / (GLdouble)height;    // ���㴰�ڵ��ݺ��
	}
	// ѡ��������ͼ���󣬲���������ϵͳ
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// ���ݴ��ڵ��ݺ�ȶ���ü����򣬲�ʹ����ͶӰ
	GLdouble w = right - left + CANVAS_PADDING * 2.0, h = top - bottom + CANVAS_PADDING * 2.0, temp;  // B ��ͼ�γ���
	if (width / w < height / h) {  // ͼ������Ӧ���ڴ�С
		// ͼ�ο����Ϊ���ֵ
		temp = w / aspectRatio / 2.0;
		glOrtho(left - CANVAS_PADDING, right + CANVAS_PADDING, -temp + h / 2.0 - CANVAS_PADDING, temp + h / 2.0 - CANVAS_PADDING, 1.0, -1.0);
	} else {
		// ͼ�θ߶���Ϊ���ֵ
		temp = h * aspectRatio / 2.0;
		glOrtho(-temp + w / 2.0 - CANVAS_PADDING, temp + w / 2.0 - CANVAS_PADDING, bottom - CANVAS_PADDING, top + CANVAS_PADDING, 1.0, -1.0);
	}
	// ѡ��ģ����ͼ���󣬲���������ϵͳ
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void Canvas::unblockOnce(bool force) {
	if (!status || force) {
		block = false;       // ȡ����������
		cv.notify_one();     // ��������ȴ�
	}
}

void Canvas::resize(int w, int h) {
	if(w)
	width = w, height = h;   // ���ô��ڸ߿�
	resized = true;          // ���ô��ڴ�С�ı��־λ
	push(UNBLOCK_QUEUE);     // ȡ����������
	unblockOnce(true);       // ȡ�������������û�����С
}

void Canvas::push(int type, int value, float x1, float y1, float x2, float y2) {
	que.push(type, x1, y1, x2, y2, value);  // �¼����
}

void Canvas::setWaitTime(int time) {
	if (time) {
		waitTime = MAX_SLEEP_TIME + MIN_SLEEP_TIME + 1 - time;  // ������ͣʱ�䣨���鷽����ʱ���С�������ģ������ü��ģ�
		cv.notify_one();     // ��������ȴ�����Ӧ����
	} else {
		que.end();           // ���ʱ��Ϊ�㣬ת�����һ֡
		block = false;       // ȡ�������ȴ�
		switchStatus(true);  // ��Ϊ����ģʽ
	}
}

void Canvas::switchStatus(bool type) {
	if (type && !status) { // �Ӹ��жϣ���ֹ����Ҫ����Դ����
		status = type;     // ���ö���״̬
		cv.notify_one();   // ���������״̬��ȡ����ͣ
		bar->msg(STATUS_STATUSBAR, L"����״̬������");
	} else if(!type && status) {
		status = type;     // ���ö���״̬
		bar->msg(STATUS_STATUSBAR, L"����״̬����ͣ");
	}
}

void Canvas::setStatusBar(StatusBar *bar) {
	this->bar = bar;  // ����״̬�����
	initBar();        // ��ʼ������
}

void Canvas::initBar(void) {
	bar->msg(ACTION_STATUSBAR, L"����");
	bar->msg(STATUS_STATUSBAR, L"����״̬������");
}