#include "StatusBar.h"

using namespace BTV;

StatusBar::StatusBar(HINSTANCE hInstance, HWND hWnd) : unresized(true) {
	parentHandle = hWnd;  // ���游���ھ��
	statusBarHandle = CreateWindowEx(NULL, STATUSCLASSNAME,  // ����״̬��
		NULL, WS_CHILD | WS_VISIBLE | SBS_SIZEGRIP,
		0, 0, 0, 0, hWnd, NULL, hInstance, 0);
	w[STATUSBAR_PARTS - 1] = -1;  // ���ұ�һ��Ĭ��ֵ
}

bool StatusBar::resize(void) {
	SetWindowPos(statusBarHandle, 0, 0, 0, 0, 0, SWP_NOZORDER);    // �Զ�����״̬����Сλ��

	RECT rect;
	GetClientRect(parentHandle, &rect);   // ��������ڴ�Сλ�ò���
	int width = rect.right - rect.left;   // ���������ڿ��
	int height = rect.bottom - rect.top;  // ���������ڸ߶�

	w[3] = rect.right * 5 / 6;   // �����ڶ���
	w[2] = rect.right * 2 / 3;   // ��������
	w[1] = rect.right / 2;
	w[0] = rect.right / 4;

	SendMessage(statusBarHandle, SB_SETPARTS, STATUSBAR_PARTS, (LPARAM)(LPINT)w);  // ��������
	msg(SIZE_STATUSBAR, L"%dx%d", width, height);                                  // ��ʾ���ڴ�С

	return unresized ? !(unresized = false) : false;  // �����Ƿ�ı����С
}
