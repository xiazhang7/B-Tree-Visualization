#pragma once

#include <Windows.h>
#include <CommCtrl.h>
#include <utility>

#define ACTION_STATUSBAR  0     // ���嶯����ʾ��״̬������һ��
#define TIME_STATUSBAR    1     // ����ʱ����ʾ��״̬������һ��
#define STATUS_STATUSBAR  2     // ���嶯��״̬��ʾ��״̬������һ��
#define DEGREE_STATUSBAR  3     // ���������ʾ��״̬������һ��
#define SIZE_STATUSBAR    4     // ���崰�ڴ�С��ʾ��״̬������һ��

#define STATUSBAR_PARTS   5     // ״̬������
#define STATUSBAR_MAX_LEN 50    // ����״̬���ַ�����������С

namespace BTV {
	class StatusBar {
	public:

		StatusBar(HINSTANCE, HWND);

		bool resize(void);         // ����״̬����С
		HWND getHandle(void)       { return statusBarHandle; }  // ����״̬�����
		template<typename ...Ts>
		void msg(int, Ts ...args); // ��ʾ��Ϣ

	private:

		HWND statusBarHandle, parentHandle;  // ���洰�ھ��
		int w[STATUSBAR_PARTS];              // �������λ����Ϣ
		bool unresized;                      // ����Ƿ�ı����С
		wchar_t str[STATUSBAR_MAX_LEN];      // ��ʱ�����ַ���

	};

	template<typename ...Ts>
	void StatusBar::msg(int pos, Ts ...args) {
		swprintf(str, STATUSBAR_MAX_LEN, std::forward<Ts>(args)...);          // �����ַ���
		SendMessage(statusBarHandle, SB_SETTEXT, (WPARAM)pos, (LPARAM)str);   // ��ʾ�ַ���
	}
}