#pragma once

#include <Windows.h>
#include "Controller.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);     // 主窗口回调函数
INT_PTR CALLBACK DialogProc(HWND, UINT, WPARAM, LPARAM);  // 控制窗口回调函数

namespace BTV {
	class Window {
	public:

		Window(HINSTANCE, LPCWSTR, HWND, Controller *);  // 初始化窗口
		~Window(void);                                   // 销毁窗口

		HWND create(void);                               // 创建窗口
		void show(int = SW_SHOWDEFAULT);                 // 使窗口可见
		HWND getHandle(void) { return handle; }          // 获得窗口句柄

		// 用于设置 WNDCLASSEX 参数的函数
		void setClassStyle(UINT style) { winClass.style = style; }        // 设置样式
		void setIcon(int id) { winClass.hIcon = loadIcon(id); }           // 设置图标
		void setIconSmall(int id) { winClass.hIconSm = loadIcon(id); }    // 设置小图标
		void setCursor(int id) { winClass.hCursor = loadCursor(id); }     // 设置鼠标指针
		void setBackground(int color) { winClass.hbrBackground = (HBRUSH)::GetStockObject(color); } // 设置背景
		void setMenuName(LPCTSTR name) { winClass.lpszMenuName = name; }  // 设置菜单名称

		// 窗口创建函数 CreateWindowEx() 的参数设置
		void setWindowStyle(DWORD style) { winStyle = style; }            // 设置窗口样式
		void setWindowStyleEx(DWORD style) { winStyleEx = style; }        // 设置扩展样式
		void setPosition(int x, int y) { this->x = x; this->y = y; }      // 设置窗口位置
		void setWidth(int w) { width = w; }                               // 设置窗口宽度
		void setHeight(int h) { height = h; }                             // 设置窗口高度
		void setParent(HWND handle) { parentHandle = handle; }            // 设置父窗口的句柄
		void setMenu(HMENU handle) { menuHandle = handle; }               // 设置菜单句柄

	private:

		HWND       handle;              // 窗口句柄
		WNDCLASSEX winClass;            // 储存 WNDCLASSEX
		DWORD      winStyle;            // 窗口样式
		DWORD      winStyleEx;          // 扩展样式
		LPCWSTR    title;               // 窗口名字
		LPCWSTR    className;           // 窗口类名
		int        x;                   // 窗口水平位置
		int        y;                   // 窗口竖直位置
		int        width;               // 窗口宽度
		int        height;              // 窗口高度
		HWND       parentHandle;        // 父窗口句柄
		HMENU      menuHandle;          // 菜单句柄
		HINSTANCE  instance;            // 窗口实例
		Controller *ctrl;               // 储存控制类

		HICON      loadIcon(int);       // 载入程序图标
		HCURSOR    loadCursor(int);     // 载入鼠标指针图标

	};
}