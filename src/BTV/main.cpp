#include <Windows.h>
#include <CommCtrl.h>
#include "Controller.h"
#include "resource.h"
#include "Graphics.h"
#include "BTree.h"
#include "Window.h"

using namespace BTV;

#define ICON_SIZE 32  // �˵�ͼ��ߴ�
#define MENU_NUM  9   // �˵���ť����

void errMsg(const wchar_t *);   // �������ڴ�����ʾ����
HWND createToolBar(HWND, HINSTANCE);  // �����˵�����

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	BTree tree;  // �½� B ��
	Controller ctrl(&tree);   // �½�������
	Window mainWindow(hInstance, TEXT("B�����ӻ�"), NULL, &ctrl);  // �½�������

	mainWindow.setWindowStyleEx(WS_EX_ACCEPTFILES);  // �����ļ��Ϸ�

	HWND mainHandle = mainWindow.create();           // ����������
	if (!mainHandle) errMsg(L"�����ڴ���ʧ�ܣ�");    // ����ʧ����Ϣ

	ctrl.setMainHandle(mainHandle);  // ���������ھ��

	Window canvasWindow(hInstance, TEXT("����"), mainHandle, NULL);              // �½���������

	canvasWindow.setClassStyle(CS_OWNDC);                                                    // �����ж����豸������
	canvasWindow.setWindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE);  // ��ֹ���������������ػ�
	canvasWindow.setWindowStyleEx(!WS_EX_CLIENTEDGE);                                        // ���������ޱ߿�

	if (!canvasWindow.create()) errMsg(L"�������ڴ���ʧ�ܣ�");    // �������ڴ���ʧ��

	ctrl.setCanvasHandle(canvasWindow.getHandle());  // ���û������

	// �������ƴ���
	HWND dialogHandle = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG_CTRL), mainHandle, DialogProc, (LPARAM)&ctrl);

	if (!dialogHandle) errMsg(L"���ƴ��ڴ���ʧ�ܣ�");    // ���ƴ��ڴ���ʧ��

	ctrl.setDialogHandle(dialogHandle);    // ���ÿ��������

	HWND toolBarHandle = createToolBar(mainHandle, hInstance); // �����˵���

	if (!toolBarHandle) errMsg(L"�˵�������ʧ�ܣ�");    // �˵�������ʧ��

	ctrl.setToolBarHandle(toolBarHandle);   // ���ò˵������
	
	// ����״̬��
	StatusBar bar(hInstance, mainHandle);
	HWND statusBarHandle = bar.getHandle();

	if (!statusBarHandle) errMsg(L"״̬������ʧ�ܣ�");   // ״̬������ʧ��

	ctrl.setStatusBar(&bar);                   // ����״̬��
	ctrl.setStatusBarHandle(statusBarHandle);  // ����״̬�����

	Canvas canvas(canvasWindow.getHandle());   // �½������������û��Ƶ���������
	tree.SetCanvas(&canvas);                   // ���� B ������
	ctrl.setCanvas(&canvas);                   // ���ÿ���������

	// ��ʾ����
	mainWindow.show();
	canvasWindow.show();
	
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(MAIN_ACCELERATOR));  // ������ټ�

	while (GetMessage(&msg, NULL, 0, 0) > 0) {     // ѭ����ȡ Windows ��Ϣ����
		if (!TranslateAccelerator(mainHandle, hAccelTable, &msg)) {
			TranslateMessage(&msg);                // ������ת��Ϊ�ַ�
			DispatchMessage(&msg);                 // ������Ϣ�� WndProc ����
		}
	}

	return msg.wParam;
}

void errMsg(const wchar_t *msg) {
	MessageBox(NULL, msg, TEXT("����"), MB_ICONERROR | MB_OK);
	exit(0);
}

HWND createToolBar(HWND hWnd, HINSTANCE hInstance)
{
	// �����˵���
	HWND toolBarHandle = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST, 0, 0, 0, 0,
		hWnd, NULL, hInstance, NULL);

	if (!toolBarHandle) return NULL; // ����ʧ��

	// ����ͼƬ�б�
	HIMAGELIST g_hImageList = ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR32, MENU_NUM, 0);
	int iBitmap = ImageList_Add(g_hImageList, LoadBitmap(hInstance, MAKEINTRESOURCE(BITMAP_MENU)), NULL);

	SendMessage(toolBarHandle, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)g_hImageList);  // ����ͼƬ�б�
	SendMessage(toolBarHandle, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);  // ����ͼƬ

	TBBUTTON buttons[MENU_NUM] =  // ���ð�ť
	{
		{ MAKELONG(iBitmap,  0), MENU_OPEN,  TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("��") },
		{ MAKELONG(iBitmap + 1, 0), MENU_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("����")},
		{ MAKELONG(iBitmap + 2, 0), MENU_START, NULL, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("��ʼ")},
		{ MAKELONG(iBitmap + 3, 0), MENU_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("��ͣ")},
		{ MAKELONG(iBitmap + 4, 0), MENU_STEP, NULL, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("����")},
		{ MAKELONG(iBitmap + 5, 0), MENU_STOP, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("ֹͣ")},
		{ MAKELONG(iBitmap + 6, 0), MENU_HELP, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("����")},
		{ MAKELONG(iBitmap + 7, 0), MENU_ABOUT, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("����")},
		{ MAKELONG(iBitmap + 8, 0), MENU_EXIT, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("�˳�")}
	};

	// ��Ӱ�ť
	SendMessage(toolBarHandle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(toolBarHandle, TB_ADDBUTTONS, (WPARAM)MENU_NUM, (LPARAM)&buttons);

	ShowWindow(toolBarHandle, TRUE);  // ��ʾ�˵���

	return toolBarHandle;  // ���ز˵������
}