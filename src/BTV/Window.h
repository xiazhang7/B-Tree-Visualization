#pragma once

#include <Windows.h>
#include "Controller.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);     // �����ڻص�����
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);  // ���ƴ��ڻص�����

namespace BTV {
	class Window {
	public:

		Window(HINSTANCE, LPCWSTR, HWND, Controller *);  // ��ʼ������
		~Window(void);                                   // ���ٴ���

		HWND create(void);                               // ��������
		void show(int = SW_SHOWDEFAULT);                 // ʹ���ڿɼ�
		HWND getHandle(void) { return handle; }          // ��ô��ھ��

		// �������� WNDCLASSEX �����ĺ���
		void setClassStyle(UINT style) { winClass.style = style; }        // ������ʽ
		void setIcon(int id) { winClass.hIcon = loadIcon(id); }           // ����ͼ��
		void setIconSmall(int id) { winClass.hIconSm = loadIcon(id); }    // ����Сͼ��
		void setCursor(int id) { winClass.hCursor = loadCursor(id); }     // �������ָ��
		void setBackground(int color) { winClass.hbrBackground = (HBRUSH)::GetStockObject(color); } // ���ñ���
		void setMenuName(LPCTSTR name) { winClass.lpszMenuName = name; }  // ���ò˵�����

		// ���ڴ������� CreateWindowEx() �Ĳ�������
		void setWindowStyle(DWORD style) { winStyle = style; }            // ���ô�����ʽ
		void setWindowStyleEx(DWORD style) { winStyleEx = style; }        // ������չ��ʽ
		void setPosition(int x, int y) { this->x = x; this->y = y; }      // ���ô���λ��
		void setWidth(int w) { width = w; }                               // ���ô��ڿ��
		void setHeight(int h) { height = h; }                             // ���ô��ڸ߶�
		void setParent(HWND handle) { parentHandle = handle; }            // ���ø����ڵľ��
		void setMenu(HMENU handle) { menuHandle = handle; }               // ���ò˵����

	private:

		HWND       handle;              // ���ھ��
		WNDCLASSEX winClass;            // ���� WNDCLASSEX
		DWORD      winStyle;            // ������ʽ
		DWORD      winStyleEx;          // ��չ��ʽ
		LPCWSTR    title;               // ��������
		LPCWSTR    className;           // ��������
		int        x;                   // ����ˮƽλ��
		int        y;                   // ������ֱλ��
		int        width;               // ���ڿ��
		int        height;              // ���ڸ߶�
		HWND       parentHandle;        // �����ھ��
		HMENU      menuHandle;          // �˵����
		HINSTANCE  instance;            // ����ʵ��
		Controller *ctrl;               // ���������

		HICON      loadIcon(int);       // �������ͼ��
		HCURSOR    loadCursor(int);     // �������ָ��ͼ��

	};
}