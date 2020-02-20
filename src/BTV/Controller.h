#pragma once

#include "File.h"
#include "BTree.h"
#include "Graphics.h"
#include "StatusBar.h"
#include <Windows.h>

#define ANIMATION_PAUSE 0  // 动画暂停
#define ANIMATION_START 1  // 动画开始
#define ANIMATION_STOP  2  // 动画结束

namespace BTV {

	class Controller {
	public:

		Controller(BTree *);               // 构造函数

		void setMainHandle(HWND hWnd)      { mainHandle = hWnd; }           // 设置主窗口句柄
		void setCanvasHandle(HWND hWnd)    { canvasHandle = hWnd; }         // 设置画布窗口句柄
		void setToolBarHandle(HWND hWnd)   { toolBarHandle = hWnd; }        // 设置动画控制栏按钮
		void setStatusBarHandle(HWND hWnd) { statusBarHandle = hWnd; }      // 设置状态栏句柄
		void setStatusBar(StatusBar *bar)  { this->bar = bar; }             // 设置状态栏
		void setCanvas(Canvas *canvas)     { this->canvas = canvas; }       // 设置画布
		bool paint(HWND hWnd)              { return hWnd != canvasHandle; } // 设置画布窗口不自动重绘
		void step(void)                    { canvas->unblockOnce(); }       // 单步演示一个画面
		void setDialogHandle(HWND);        // 设置控制窗口句柄
		void open(const wchar_t * = NULL); // 打开文件
		void save(void);                   // 保存文件
		void drag(HDROP);                  // 处理拖拽文件
		void command(int);                 // 处理按钮事件
		void resize(HWND, int, int);       // 适应窗口大小
		void speed(void);                  // 设置动画速度
		void animation(int);               // 切换动画状态

	private:

		HWND mainHandle, canvasHandle, dialogHandle, sliderHandle, toolBarHandle, statusBarHandle;  // 窗口句柄
		Canvas *canvas;      // 画布
		BTree *tree;         // B 树类
		File file;           // 文件类
		StatusBar *bar;      // 状态栏类
		std::fstream fstm;   // 文件流
		wchar_t s[MAX_PATH]; // 临时储存字符串

		bool handleCommand(int &, int);  // 处理按钮事件
		void setToolBarButton(bool);     // 设置菜单栏按钮状态

	};
}