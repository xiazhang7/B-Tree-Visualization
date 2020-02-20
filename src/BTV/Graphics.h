#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Queue.h"
#include "StatusBar.h"

#define CANVAS_PADDING   0.1   // 画布内边距
#define NODE_PADDING     0.05f // 节点内边距
#define SLEEP_TIME       500   // 动画演示默认间隔时间 （毫秒）
#define MIN_SLEEP_TIME   50    // 最小间隔时间
#define MAX_SLEEP_TIME   2000  // 最大间隔时间
#define MAX_CHAR         128   // 最大字符 ASCII 编码
#define MAX_STR_LEN      20    // 最大字符长度

namespace BTV {
	class Canvas {
	public:

		Canvas(HWND);   // 初始化画布
		~Canvas(void);  // 销毁画布

		void resize(int, int);           // 重设窗口大小
		void push(int, int = 1, float = 1.0f, float = 1.0f, float = 1.0f, float = 1.0f); // 设置渲染事件
		void setWaitTime(int);           // 设置动画间隔时间
		void switchStatus(bool);         // 切换动画状态
		void setStatusBar(StatusBar *);  // 设置状态栏句柄
		void unblockOnce(bool = false);  // 取消阻塞一次

	private:

		char str[MAX_STR_LEN];              // 储存字符
		Queue que;                          // 事件队列
		StatusBar *bar;                     // 状态栏
		GLuint font;                        // 字符显示列表开始位置
		GLYPHMETRICSFLOAT gmf[MAX_CHAR];    // 记录字符信息
		HDC hDC;                            // 设备描述符
		HGLRC hRC;                          // 资源上下文
		HWND hWnd;                          // 画布窗口句柄
		std::thread renderThread;           // 渲染线程
		std::mutex mtx;                     // 休眠锁
		std::condition_variable cv;         // 条件变量
		int waitTime;                       // 演示间隔时间
		volatile bool block;                // 标记是否继续阻塞
		volatile bool status;               // 设置演示状态
		volatile bool loop;                 // 循环渲染标志
		volatile bool resized;              // 窗口大小改变标志
		int width, height;                  // 画布窗口高宽、父窗口高度
		float lastTime;                     // 记录上次操作用时
		GLdouble aspectRatio;               // 窗口纵横比
		GLdouble left, right, bottom, top;  // B 树图形大小位置
		std::chrono::time_point<std::chrono::steady_clock> start;  // 记录动作开始时间
		std::chrono::duration<double, std::milli> duration;        // 保存时间差

		void init(void);                    // 初始化渲染窗口
		void render(void);                  // 渲染函数
		void handleResize(void);            // 处理窗口大小改变
		void initBar(void);                 // 初始化状态栏显示文字

	};
}