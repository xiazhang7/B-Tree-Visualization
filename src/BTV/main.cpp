#include <Windows.h>
#include <CommCtrl.h>
#include "Controller.h"
#include "resource.h"
#include "Graphics.h"
#include "BTree.h"
#include "Window.h"

using namespace BTV;

#define ICON_SIZE 32  // 菜单图标尺寸
#define MENU_NUM  9   // 菜单按钮数量

void errMsg(const wchar_t *);   // 创建窗口错误提示函数
HWND createToolBar(HWND, HINSTANCE);  // 创建菜单函数

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	BTree tree;  // 新建 B 树
	Controller ctrl(&tree);   // 新建控制器
	Window mainWindow(hInstance, TEXT("B树可视化"), NULL, &ctrl);  // 新建主窗口

	mainWindow.setWindowStyleEx(WS_EX_ACCEPTFILES);  // 允许文件拖放

	HWND mainHandle = mainWindow.create();           // 创建主窗口
	if (!mainHandle) errMsg(L"主窗口创建失败！");    // 创建失败消息

	ctrl.setMainHandle(mainHandle);  // 设置主窗口句柄

	Window canvasWindow(hInstance, TEXT("画布"), mainHandle, NULL);              // 新建画布窗口

	canvasWindow.setClassStyle(CS_OWNDC);                                                    // 窗口有独立设备上下文
	canvasWindow.setWindowStyle(WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE);  // 防止画布被其他窗口重绘
	canvasWindow.setWindowStyleEx(!WS_EX_CLIENTEDGE);                                        // 画布窗口无边框

	if (!canvasWindow.create()) errMsg(L"画布窗口创建失败！");    // 画布窗口创建失败

	ctrl.setCanvasHandle(canvasWindow.getHandle());  // 设置画布句柄

	// 创建控制窗口
	HWND dialogHandle = CreateDialogParam(hInstance, MAKEINTRESOURCE(DIALOG_CTRL), mainHandle, DialogProc, (LPARAM)&ctrl);

	if (!dialogHandle) errMsg(L"控制窗口创建失败！");    // 控制窗口创建失败

	ctrl.setDialogHandle(dialogHandle);    // 设置控制器句柄

	HWND toolBarHandle = createToolBar(mainHandle, hInstance); // 创建菜单栏

	if (!toolBarHandle) errMsg(L"菜单栏创建失败！");    // 菜单栏创建失败

	ctrl.setToolBarHandle(toolBarHandle);   // 设置菜单栏句柄
	
	// 创建状态栏
	StatusBar bar(hInstance, mainHandle);
	HWND statusBarHandle = bar.getHandle();

	if (!statusBarHandle) errMsg(L"状态栏创建失败！");   // 状态栏创建失败

	ctrl.setStatusBar(&bar);                   // 设置状态栏
	ctrl.setStatusBarHandle(statusBarHandle);  // 设置状态栏句柄

	Canvas canvas(canvasWindow.getHandle());   // 新建画布，并设置绘制到画布窗口
	tree.SetCanvas(&canvas);                   // 设置 B 树画布
	ctrl.setCanvas(&canvas);                   // 设置控制器画布

	// 显示窗口
	mainWindow.show();
	canvasWindow.show();
	
	MSG msg;
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(MAIN_ACCELERATOR));  // 载入加速键

	while (GetMessage(&msg, NULL, 0, 0) > 0) {     // 循环读取 Windows 消息队列
		if (!TranslateAccelerator(mainHandle, hAccelTable, &msg)) {
			TranslateMessage(&msg);                // 将按键转换为字符
			DispatchMessage(&msg);                 // 发送消息到 WndProc 函数
		}
	}

	return msg.wParam;
}

void errMsg(const wchar_t *msg) {
	MessageBox(NULL, msg, TEXT("错误"), MB_ICONERROR | MB_OK);
	exit(0);
}

HWND createToolBar(HWND hWnd, HINSTANCE hInstance)
{
	// 创建菜单栏
	HWND toolBarHandle = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD | TBSTYLE_WRAPABLE | TBSTYLE_LIST, 0, 0, 0, 0,
		hWnd, NULL, hInstance, NULL);

	if (!toolBarHandle) return NULL; // 创建失败

	// 创建图片列表
	HIMAGELIST g_hImageList = ImageList_Create(ICON_SIZE, ICON_SIZE, ILC_COLOR32, MENU_NUM, 0);
	int iBitmap = ImageList_Add(g_hImageList, LoadBitmap(hInstance, MAKEINTRESOURCE(BITMAP_MENU)), NULL);

	SendMessage(toolBarHandle, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)g_hImageList);  // 设置图片列表
	SendMessage(toolBarHandle, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL);  // 加载图片

	TBBUTTON buttons[MENU_NUM] =  // 设置按钮
	{
		{ MAKELONG(iBitmap,  0), MENU_OPEN,  TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("打开") },
		{ MAKELONG(iBitmap + 1, 0), MENU_SAVE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("保存")},
		{ MAKELONG(iBitmap + 2, 0), MENU_START, NULL, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("开始")},
		{ MAKELONG(iBitmap + 3, 0), MENU_PAUSE, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("暂停")},
		{ MAKELONG(iBitmap + 4, 0), MENU_STEP, NULL, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("单步")},
		{ MAKELONG(iBitmap + 5, 0), MENU_STOP, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("停止")},
		{ MAKELONG(iBitmap + 6, 0), MENU_HELP, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("帮助")},
		{ MAKELONG(iBitmap + 7, 0), MENU_ABOUT, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("关于")},
		{ MAKELONG(iBitmap + 8, 0), MENU_EXIT, TBSTATE_ENABLED, BTNS_AUTOSIZE, {0}, 0, (INT_PTR)TEXT("退出")}
	};

	// 添加按钮
	SendMessage(toolBarHandle, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
	SendMessage(toolBarHandle, TB_ADDBUTTONS, (WPARAM)MENU_NUM, (LPARAM)&buttons);

	ShowWindow(toolBarHandle, TRUE);  // 显示菜单栏

	return toolBarHandle;  // 返回菜单栏句柄
}