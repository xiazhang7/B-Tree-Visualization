#include "Controller.h"
#include "resource.h"
#include <commctrl.h>

using namespace BTV;

Controller::Controller(BTree *tree) : tree(tree) {  // ��ʼ�������� B ��ָ��

}

void Controller::open(const wchar_t *path) {
	bool result = false;
	switch (file.openFile(fstm, path)) {  // ��ֻ���ļ���
	case FILE_FAIL:
		MessageBox(mainHandle, TEXT("���ļ�ʧ�ܣ�"), TEXT("����"), MB_ICONERROR | MB_OK);  // ��ʧ��
	case FILE_CANCEL:  // �û�ȡ������
		return;
	case FILE_TEXT_SUCCESS:               // �ɹ����ı��ļ�
		result = tree->Open(fstm, false); // ���� B ���ڲ���ȡ
		break;
	case FILE_BINARY_SUCCESS:             // �ɹ��򿪶������ļ�
		result = tree->Open(fstm, true);  // ���� B ���ڲ���ȡ
		break;
	}
	canvas->push(STATUS_LOAD, result);    // ״̬����ʾ���ļ����
	if (!result) MessageBox(mainHandle, TEXT("�ļ��𻵻��ʽ���ԣ�"), TEXT("��ȡ�ļ�ʧ��"), MB_ICONERROR | MB_OK); // B ���ڲ���ȡʧ��
}

void Controller::save(void) {
	bool result = false;
	switch (file.saveFile(fstm)) {        // ��ֻд�ļ���
	case FILE_FAIL:
		MessageBox(mainHandle, TEXT("�����ļ�ʧ�ܣ�"), TEXT("����"), MB_ICONERROR | MB_OK);  // ��ʧ��
		break;
	case FILE_BINARY_SUCCESS:             // �ɹ��򿪶������ļ�
		result = tree->Save(fstm);
		if (result) MessageBox(mainHandle, TEXT("�ļ�����ɹ���"), TEXT("�ɹ�"), MB_OK);  // д��ɹ�
		else MessageBox(mainHandle, TEXT("�ļ�д��ʧ�ܣ�"), TEXT("ʧ��"), MB_ICONERROR | MB_OK);    // д��ʧ��
		break;
	case FILE_CANCEL:
		return;
	}
	canvas->push(STATUS_SAVE, result);    // ״̬����ʾ�����ļ����
}

void Controller::drag(HDROP hDrop) {
	DragQueryFile(hDrop, 0, s, sizeof(s));                    // ��ȡ��һ���ļ�·��
	if (!(GetFileAttributes(s) & FILE_ATTRIBUTE_DIRECTORY))   // ��������ļ��оʹ�
		open(s);
	DragFinish(hDrop);  // ��������
}

void Controller::command(int cmd) {
	static int result;
	switch (cmd) {
	case BUTTON_INSERT:  // ���밴ť
		if (handleCommand(result, EDIT_INSERT)) // �����Ӧ�ı������ݣ���ͬ
			if (!tree->Insert(result))          // ���ݺϷ���ִ�ж�Ӧ����
				MessageBox(mainHandle, TEXT("�������ô���"), TEXT("����ʧ��"), MB_ICONERROR | MB_OK);
		break;
	case BUTTON_DELETE:  // ɾ����ť
		if (handleCommand(result, EDIT_DELETE)) tree->Delete(result);
		break;
	case BUTTON_SEARCH:  // ������ť
		if (handleCommand(result, EDIT_SEARCH)) tree->Search(result);
		break;
	case BUTTON_PRINT:   // ������ť
		tree->Print(false);
		break;
	case BUTTON_CLEAR:   // ��հ�ť
		tree->Destory();
		break;
	case BUTTON_DEGREE:  // ���ý�����ť
		if (handleCommand(result, EDIT_DEGREE))
			if (!tree->SetDegree(result))
				MessageBox(mainHandle, TEXT("�����Ƿ���"), TEXT("��������ʧ��"), MB_ICONERROR | MB_OK);
		break;
	}
}

void Controller::resize(HWND hWnd, int width, int height) {
	if (hWnd != mainHandle) return;             // ���������ڵĴ�С�ı䲻��Ӧ

	SetWindowPos(toolBarHandle, 0, 0, 0, 0, 0, SWP_NOZORDER);   // �Զ����ò˵�����С
	
	if (bar->resize()) {           // ���״̬����Сû�иı��
		tree->SetStatusBar(bar);   // ���� B ��״̬��
		canvas->setStatusBar(bar); // ���û���״̬��
	}

	RECT rect;
	GetWindowRect(dialogHandle, &rect);            // ��ÿ��ƴ��ڴ�С����
	int dialogHeight = rect.bottom - rect.top;     // ���ƴ��ڸ߶�
	int dialogWidth  = rect.right - rect.left;     // ���ƴ��ڿ��
	GetWindowRect(statusBarHandle, &rect);         // ���״̬����С����
	int statusHeight = rect.bottom - rect.top;     // ״̬���߶�
	int dialogPosX   = (width - dialogWidth) / 2;  // ���ƴ��ں�����
	height -= dialogHeight + statusHeight;         // ���ƴ�������������
	SetWindowPos(dialogHandle, 0, dialogPosX, height, dialogWidth, dialogHeight, SWP_NOZORDER);  // ���ÿ��ƴ��ڴ�Сλ��

	GetWindowRect(toolBarHandle, &rect);           // ��ò˵���λ�ô�С
	int toolBarHeight = rect.bottom - rect.top;    // �˵����߶�
	height -= toolBarHeight;                       // �����߶�
	SetWindowPos(canvasHandle, 0, 0, toolBarHeight, width, height, SWP_NOZORDER);  // ���û�����Сλ��
	canvas->resize(width, height);   // ���� OpenGL ��Ⱦ��Сλ��
}

bool Controller::handleCommand(int &result, int id) {
	HWND hWnd = GetDlgItem(dialogHandle, id);  // ��ð�ť��Ӧ�ı�����
	result = GetWindowTextLength(hWnd);        // ����ı�����
	if (result && result < 10) {               // ���Ȳ���Ϊ���Ҳ��ܳ��� 9 λ������ int ��С���ƣ�
		GetWindowText(hWnd, s, 10);            // ��ȡ�ı�
		result = 0;                            // ��������
		bool f = false;                        // ����λ, true Ϊ��
		int i = 0;
		for (; s[i]; i++) {                    // ���ַ����������
			if ('0' <= s[i] && s[i] <= '9') result = result * 10 + s[i] - '0';
			else if (!i && s[i] == '-') f = true;
			else break;
		}
		s[0] = NULL;
		SetWindowText(hWnd, s);                // ���������ı�
		if (!s[i] && !(f && i == 1)) {         // ���ֺϷ���ת���ɹ�
			if (f) result = -result;           // ���Ϸ���
			return true;                       // ���سɹ�
		}
	}
	MessageBox(mainHandle, TEXT("�Ƿ����룡"), TEXT("����"), MB_ICONERROR | MB_OK);  // ʧ�ܣ���ʾ����
	return false;   // ����ʧ��
}

void Controller::setDialogHandle(HWND hWnd) {
	dialogHandle = hWnd;  // ���ÿ��ƴ��ھ��
	sliderHandle = GetDlgItem(hWnd, SLIDER_SPEED);  // �����ٶȿ��ƻ�����
	SendMessage(sliderHandle, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(1, MAX_SLEEP_TIME + 1));  // �����ٶȿ��Ʒ�Χ����һΪ�˷�ֹʱ����㣩
	SendMessage(sliderHandle, TBM_SETPOS, (WPARAM)1, (LPARAM)(int)(MAX_SLEEP_TIME + MIN_SLEEP_TIME + 1 - SLEEP_TIME));
}

void Controller::speed(void) {
	canvas->setWaitTime(SendMessage(sliderHandle, TBM_GETPOS, 0, 0));  // ��ȡ������ֵ���͸�����
}

void Controller::animation(int type) {
	switch(type) {
	case ANIMATION_START:
		setToolBarButton(true);
		canvas->switchStatus(type);  // �л�������״̬
		break;
	case ANIMATION_PAUSE:
		setToolBarButton(false);
		canvas->switchStatus(type);  // �л�����ͣ״̬
		break;
	case ANIMATION_STOP:
		setToolBarButton(true);
		canvas->setWaitTime(0);      // �������һ������
		break;
	}
}

void Controller::setToolBarButton(bool start) {  // ���ò˵�����ť���á�����
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_START, start ? NULL : MAKELONG(TBSTATE_ENABLED, 0));
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_PAUSE, start ? MAKELONG(TBSTATE_ENABLED, 0) : NULL);
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_STEP, start ? NULL : MAKELONG(TBSTATE_ENABLED, 0));
}