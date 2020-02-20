#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <utility>

#define ACTION_STATUSBAR  0     // 定义动作显示在状态栏的哪一栏
#define TIME_STATUSBAR    1     // 定义时间显示在状态栏的哪一栏
#define STATUS_STATUSBAR  2     // 定义动画状态显示在状态栏的哪一栏
#define DEGREE_STATUSBAR  3     // 定义阶数显示在状态栏的哪一栏
#define SIZE_STATUSBAR    4     // 定义窗口大小显示在状态栏的哪一栏

#define STATUSBAR_PARTS   5     // 状态栏栏数
#define STATUSBAR_MAX_LEN 50    // 定义状态栏字符串储存区大小

namespace BTV {
	class StatusBar {
	public:

		StatusBar(HINSTANCE, HWND);

		bool resize(void);         // 重置状态栏大小
		HWND getHandle(void)       { return statusBarHandle; }  // 返回状态栏句柄
		template<typename ...Ts>
		void msg(int, Ts ...args); // 显示信息

	private:

		HWND statusBarHandle, parentHandle;  // 储存窗口句柄
		int w[STATUSBAR_PARTS];              // 储存分栏位置信息
		bool unresized;                      // 标记是否改变过大小
		wchar_t str[STATUSBAR_MAX_LEN];      // 临时储存字符串

	};

	template<typename ...Ts>
	void StatusBar::msg(int pos, Ts ...args) {
		swprintf(str, STATUSBAR_MAX_LEN, std::forward<Ts>(args)...);          // 生成字符串
		SendMessage(statusBarHandle, SB_SETTEXT, (WPARAM)pos, (LPARAM)str);   // 显示字符串
	}
}