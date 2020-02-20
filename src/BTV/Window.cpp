#include "Window.h"
#include "resource.h"

using namespace BTV;

LRESULT CALLBACK WndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static Controller *ctrl = NULL;
	if (Message == WM_NCCREATE) {      // ���ڴ���ǰ���յ�����Ϣ
		if(!ctrl) ctrl = (Controller *)(((CREATESTRUCT*)lParam)->lpCreateParams);  // ���������ڿ�����
		return DefWindowProc(hWnd, Message, wParam, lParam);
	}
	if (!ctrl) return DefWindowProc(hWnd, Message, wParam, lParam);  // ������������ǰ��������Ϣ
	switch (Message) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case MENU_OPEN:   // �˵�����ټ����ļ�
			ctrl->open();
			break;
		case MENU_SAVE:   // �˵�����ټ������ļ�
			ctrl->save();
			break;
		case MENU_EXIT:   // �˵�����ټ��˳�����
			PostQuitMessage(0);
			break;
		case MENU_HELP:   // �˵�����ټ���ʾ����
			MessageBox(hWnd, TEXT(
				"�ļ�������\n"
				"�򿪣����������ű�������������ļ�\n"
				"���棺���浱ǰ����Ϊ�������ļ�\n\n"
				"���ܣ�\n"
				"��ʼ����ʼ������ʾ\n"
				"��ͣ����ͣ������ʾ\n"
				"������������ʾ B ������\n"
				"ֹͣ��������������ʾ B ��������̬\n"
				"�ٶȣ����½ǻ���ɵ�\n\n"
				"��ݼ���\n"
				"��ʼ������F9\n"
				"��ͣ������F10\n"
				"����������F11\n"
				"����������F12\n"
				"���ļ���Ctrl+O\n"
				"�����ļ���Ctrl+S\n"
				"��ȡ������Ctrl+H\n"
				"�鿴���ڣ�Ctrl+I\n"
				"�˳�����Ctrl+Q\n\n"
				"Tips����ֱ����ק�ļ������������ļ�"
			), TEXT("����"), MB_OK);
			break;
		case MENU_ABOUT:  // �˵�����ټ���ʾ����ҳ��
			MessageBox(hWnd, TEXT(
				"������ѧ\n"
				"���������ѧԺ\n"
				"���1813 ����\n"
				"Copyright 2019\n\n"
				"ָ����ʦ������"
			), TEXT("����"), MB_OK);
			break;
		case MENU_START:  // �˵�����ټ���ʼ����
			ctrl->animation(ANIMATION_START);
			break;
		case MENU_PAUSE:  // �˵�����ټ���ʾ��ͣ����
			ctrl->animation(ANIMATION_PAUSE);
			break;
		case MENU_STEP:   // �˵�����ټ�������ʾ����
			ctrl->step();
			break;
		case MENU_STOP:   // �˵�����ټ���ʾ��������
			ctrl->animation(ANIMATION_STOP);
			break;
		}
		break;
	case WM_SIZE:   // ���ڴ�С�����ı�
		ctrl->resize(hWnd, LOWORD(lParam), HIWORD(lParam));
		break;
	case WM_ERASEBKGND:
		if (ctrl->paint(hWnd)) return DefWindowProc(hWnd, Message, wParam, lParam);  // ����������ھ��Զ��ػ�
		else break;  // ����ǻ������ھ������Լ��ػ棬һ�������������ȡ����ٲ���Ҫ��Դ���ģ���һ����ɷ�ֹ������ͣʱ�����ϵ���Ļ�⻭������յ�����
	case WM_DESTROY:          // ���ڱ��ݻ�
		PostQuitMessage(0);   // �����˳���Ϣ
		break;
	case WM_GETMINMAXINFO: {
		MINMAXINFO *mminfo = (PMINMAXINFO)lParam;
		mminfo->ptMinTrackSize.x = 1150;  // ������С���ڿ��
		mminfo->ptMinTrackSize.y = 300;   // ������С���ڸ߶�
		break;
	}
	case WM_DROPFILES:
		ctrl->drag((HDROP)wParam);
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);  // Ĭ�ϴ�����Ϣ����
	}
	return 0;
}

INT_PTR CALLBACK DialogProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	static Controller *ctrl = NULL;
	if (Message == WM_INITDIALOG){      // �Ի����ڴ���ǰ���ܵ�����Ϣ
		ctrl = (Controller*)lParam;     // ������ƴ��ڿ�����
		return true;
	}
	if (!ctrl) return false;            // ������������ǰ��������Ϣ
	switch (Message) {
	case WM_COMMAND:                    // ���հ�ť��Ϣ
		ctrl->command(LOWORD(wParam));
		break;
	case WM_HSCROLL:                    // ���ջ�����Ϣ
		ctrl->speed();
		break;
	default:                            // ������Ϣ������
		return false;
	}
	return true;
}

//����ʹ�ó�ʼ���б�����ʼ��һЩ��������߳�������Ч�ʣ����ڴ�Сλ��ΪĬ��ֵ
Window::Window(HINSTANCE hInst, LPCWSTR name, HWND hParent, Controller *ctrl) : handle(NULL), instance(hInst),
                                                                                winStyle(WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN),
                                                                                winStyleEx(WS_EX_CLIENTEDGE), x(CW_USEDEFAULT), y(CW_USEDEFAULT),
                                                                                width(CW_USEDEFAULT), height(CW_USEDEFAULT), ctrl(ctrl),
                                                                                parentHandle(hParent), menuHandle(NULL) {

	title = className = name;                                           // ���ô������ֺ�����

	// WNDCLASSEX ��������
	memset(&winClass, 0, sizeof(winClass));                             // �ṹ������

	winClass.cbSize        = sizeof(WNDCLASSEX);                        // WNDCLASSEX �ṹ���С
	winClass.lpfnWndProc   = WndProc;                                   // ���ڻص�����ָ��
	winClass.hInstance     = instance;                                  // ����ʵ��
	winClass.hIcon         = LoadIcon(instance, IDI_APPLICATION);       // Ĭ��ͼ��
	winClass.hCursor       = LoadCursor(0, IDC_ARROW);                  // Ĭ��ָ��
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);       // Ĭ�Ϻ�ɫ����
	winClass.lpszClassName = className;                                 // ����
	winClass.hIconSm       = LoadIcon(instance, IDI_APPLICATION);       // Ĭ��Сͼ��
}

Window::~Window(void) {
	UnregisterClass(className, instance);         // ȡ����ע��
}

HWND Window::create(void) {
	if (!RegisterClassEx(&winClass)) return NULL;    // ע����

	handle = CreateWindowEx(
		winStyleEx,           // ��չ��ʽ
		className,            // ����
		title,                // ���ڱ���
		winStyle,             // ������ʽ
		x,                    // ����ˮƽλ��
		y,                    // ������ֱλ��
		width,                // ���ڿ��
		height,               // ���ڸ߶�
		parentHandle,         // �����ھ��
		menuHandle,           // �˵����
		instance,             // ����ʵ��
		(LPVOID)ctrl          // ��������
	);

	return handle;
}

void Window::show(int cmdShow) {
	ShowWindow(handle, cmdShow);     // ������ʾ����
	UpdateWindow(handle);            // ���´���
}

HICON Window::loadIcon(int id) {
	return (HICON)LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_ICON, 0, 0, LR_DEFAULTSIZE);      // ���س���ͼ����Դ
}

HCURSOR Window::loadCursor(int id) {
	return (HCURSOR)LoadImage(instance, MAKEINTRESOURCE(id), IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE);  // ����ָ��ͼ����Դ
}