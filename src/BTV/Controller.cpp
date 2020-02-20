#include "Controller.h"
#include "resource.h"
#include <commctrl.h>

using namespace BTV;

Controller::Controller(BTree *tree) : tree(tree) {  // 初始化，存入 B 树指针

}

void Controller::open(const wchar_t *path) {
	bool result = false;
	switch (file.openFile(fstm, path)) {  // 打开只读文件流
	case FILE_FAIL:
		MessageBox(mainHandle, TEXT("打开文件失败！"), TEXT("错误"), MB_ICONERROR | MB_OK);  // 打开失败
	case FILE_CANCEL:  // 用户取消操作
		return;
	case FILE_TEXT_SUCCESS:               // 成功打开文本文件
		result = tree->Open(fstm, false); // 传给 B 树内部读取
		break;
	case FILE_BINARY_SUCCESS:             // 成功打开二进制文件
		result = tree->Open(fstm, true);  // 传给 B 树内部读取
		break;
	}
	canvas->push(STATUS_LOAD, result);    // 状态栏显示打开文件结果
	if (!result) MessageBox(mainHandle, TEXT("文件损坏或格式不对！"), TEXT("读取文件失败"), MB_ICONERROR | MB_OK); // B 树内部读取失败
}

void Controller::save(void) {
	bool result = false;
	switch (file.saveFile(fstm)) {        // 打开只写文件流
	case FILE_FAIL:
		MessageBox(mainHandle, TEXT("保存文件失败！"), TEXT("错误"), MB_ICONERROR | MB_OK);  // 打开失败
		break;
	case FILE_BINARY_SUCCESS:             // 成功打开二进制文件
		result = tree->Save(fstm);
		if (result) MessageBox(mainHandle, TEXT("文件保存成功！"), TEXT("成功"), MB_OK);  // 写入成功
		else MessageBox(mainHandle, TEXT("文件写入失败！"), TEXT("失败"), MB_ICONERROR | MB_OK);    // 写入失败
		break;
	case FILE_CANCEL:
		return;
	}
	canvas->push(STATUS_SAVE, result);    // 状态栏显示保存文件结果
}

void Controller::drag(HDROP hDrop) {
	DragQueryFile(hDrop, 0, s, sizeof(s));                    // 获取第一个文件路径
	if (!(GetFileAttributes(s) & FILE_ATTRIBUTE_DIRECTORY))   // 如果不是文件夹就打开
		open(s);
	DragFinish(hDrop);  // 结束操作
}

void Controller::command(int cmd) {
	static int result;
	switch (cmd) {
	case BUTTON_INSERT:  // 插入按钮
		if (handleCommand(result, EDIT_INSERT)) // 处理对应文本框数据，下同
			if (!tree->Insert(result))          // 数据合法则执行对应操作
				MessageBox(mainHandle, TEXT("阶数设置错误！"), TEXT("插入失败"), MB_ICONERROR | MB_OK);
		break;
	case BUTTON_DELETE:  // 删除按钮
		if (handleCommand(result, EDIT_DELETE)) tree->Delete(result);
		break;
	case BUTTON_SEARCH:  // 搜索按钮
		if (handleCommand(result, EDIT_SEARCH)) tree->Search(result);
		break;
	case BUTTON_PRINT:   // 遍历按钮
		tree->Print(false);
		break;
	case BUTTON_CLEAR:   // 清空按钮
		tree->Destory();
		break;
	case BUTTON_DEGREE:  // 设置阶数按钮
		if (handleCommand(result, EDIT_DEGREE))
			if (!tree->SetDegree(result))
				MessageBox(mainHandle, TEXT("阶数非法！"), TEXT("阶数设置失败"), MB_ICONERROR | MB_OK);
		break;
	}
}

void Controller::resize(HWND hWnd, int width, int height) {
	if (hWnd != mainHandle) return;             // 不是主窗口的大小改变不响应

	SetWindowPos(toolBarHandle, 0, 0, 0, 0, 0, SWP_NOZORDER);   // 自动设置菜单栏大小
	
	if (bar->resize()) {           // 如果状态栏大小没有改变过
		tree->SetStatusBar(bar);   // 设置 B 树状态栏
		canvas->setStatusBar(bar); // 设置画布状态栏
	}

	RECT rect;
	GetWindowRect(dialogHandle, &rect);            // 获得控制窗口大小参数
	int dialogHeight = rect.bottom - rect.top;     // 控制窗口高度
	int dialogWidth  = rect.right - rect.left;     // 控制窗口宽度
	GetWindowRect(statusBarHandle, &rect);         // 获得状态栏大小参数
	int statusHeight = rect.bottom - rect.top;     // 状态栏高度
	int dialogPosX   = (width - dialogWidth) / 2;  // 控制窗口横坐标
	height -= dialogHeight + statusHeight;         // 控制窗口所在纵坐标
	SetWindowPos(dialogHandle, 0, dialogPosX, height, dialogWidth, dialogHeight, SWP_NOZORDER);  // 设置控制窗口大小位置

	GetWindowRect(toolBarHandle, &rect);           // 获得菜单栏位置大小
	int toolBarHeight = rect.bottom - rect.top;    // 菜单栏高度
	height -= toolBarHeight;                       // 画布高度
	SetWindowPos(canvasHandle, 0, 0, toolBarHeight, width, height, SWP_NOZORDER);  // 设置画布大小位置
	canvas->resize(width, height);   // 重设 OpenGL 渲染大小位置
}

bool Controller::handleCommand(int &result, int id) {
	HWND hWnd = GetDlgItem(dialogHandle, id);  // 获得按钮对应文本框句柄
	result = GetWindowTextLength(hWnd);        // 获得文本长度
	if (result && result < 10) {               // 长度不能为零且不能超过 9 位（考虑 int 大小限制）
		GetWindowText(hWnd, s, 10);            // 获取文本
		result = 0;                            // 保存数字
		bool f = false;                        // 符号位, true 为负
		int i = 0;
		for (; s[i]; i++) {                    // 将字符串变成数字
			if ('0' <= s[i] && s[i] <= '9') result = result * 10 + s[i] - '0';
			else if (!i && s[i] == '-') f = true;
			else break;
		}
		s[0] = NULL;
		SetWindowText(hWnd, s);                // 清空输入框文本
		if (!s[i] && !(f && i == 1)) {         // 数字合法，转换成功
			if (f) result = -result;           // 带上符号
			return true;                       // 返回成功
		}
	}
	MessageBox(mainHandle, TEXT("非法输入！"), TEXT("错误"), MB_ICONERROR | MB_OK);  // 失败，提示错误
	return false;   // 返回失败
}

void Controller::setDialogHandle(HWND hWnd) {
	dialogHandle = hWnd;  // 设置控制窗口句柄
	sliderHandle = GetDlgItem(hWnd, SLIDER_SPEED);  // 设置速度控制滑块句柄
	SendMessage(sliderHandle, TBM_SETRANGE, TRUE, (LPARAM)MAKELONG(1, MAX_SLEEP_TIME + 1));  // 设置速度控制范围（加一为了防止时间变零）
	SendMessage(sliderHandle, TBM_SETPOS, (WPARAM)1, (LPARAM)(int)(MAX_SLEEP_TIME + MIN_SLEEP_TIME + 1 - SLEEP_TIME));
}

void Controller::speed(void) {
	canvas->setWaitTime(SendMessage(sliderHandle, TBM_GETPOS, 0, 0));  // 获取滑块数值发送给画布
}

void Controller::animation(int type) {
	switch(type) {
	case ANIMATION_START:
		setToolBarButton(true);
		canvas->switchStatus(type);  // 切换到运行状态
		break;
	case ANIMATION_PAUSE:
		setToolBarButton(false);
		canvas->switchStatus(type);  // 切换到暂停状态
		break;
	case ANIMATION_STOP:
		setToolBarButton(true);
		canvas->setWaitTime(0);      // 跳到最后一副画面
		break;
	}
}

void Controller::setToolBarButton(bool start) {  // 设置菜单栏按钮启用、禁用
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_START, start ? NULL : MAKELONG(TBSTATE_ENABLED, 0));
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_PAUSE, start ? MAKELONG(TBSTATE_ENABLED, 0) : NULL);
	SendMessage(toolBarHandle, TB_SETSTATE, (WPARAM)MENU_STEP, start ? NULL : MAKELONG(TBSTATE_ENABLED, 0));
}