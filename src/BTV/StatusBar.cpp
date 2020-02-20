#include "StatusBar.h"

using namespace BTV;

StatusBar::StatusBar(HINSTANCE hInstance, HWND hWnd) : unresized(true) {
	parentHandle = hWnd;  // 保存父窗口句柄
	statusBarHandle = CreateWindowEx(NULL, STATUSCLASSNAME,  // 创建状态栏
		NULL, WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,
		0, 0, 0, 0, hWnd, NULL, hInstance, 0);
	w[STATUSBAR_PARTS - 1] = -1;  // 最右边一栏默认值
}

bool StatusBar::resize(void) {
	SetWindowPos(statusBarHandle, 0, 0, 0, 0, 0, SWP_NOZORDER);    // 自动设置状态栏大小位置

	RECT rect;
	GetClientRect(parentHandle, &rect);   // 获得主窗口大小位置参数
	int width = rect.right - rect.left;   // 计算主窗口宽度
	int height = rect.bottom - rect.top;  // 计算主窗口高度

	w[3] = rect.right * 5 / 6;   // 倒数第二栏
	w[2] = rect.right * 2 / 3;   // 依此类推
	w[1] = rect.right / 2;
	w[0] = rect.right / 4;

	SendMessage(statusBarHandle, SB_SETPARTS, STATUSBAR_PARTS, (LPARAM)(LPINT)w);  // 设置栏数
	msg(SIZE_STATUSBAR, L"%dx%d", width, height);                                  // 显示窗口大小

	return unresized ? !(unresized = false) : false;  // 返回是否改变过大小
}
