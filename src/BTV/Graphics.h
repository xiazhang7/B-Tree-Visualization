#pragma once

#include <Windows.h>
#include <gl/GL.h>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "Queue.h"
#include "StatusBar.h"

#define CANVAS_PADDING   0.1   // �����ڱ߾�
#define NODE_PADDING     0.05f // �ڵ��ڱ߾�
#define SLEEP_TIME       500   // ������ʾĬ�ϼ��ʱ�� �����룩
#define MIN_SLEEP_TIME   50    // ��С���ʱ��
#define MAX_SLEEP_TIME   2000  // �����ʱ��
#define MAX_CHAR         128   // ����ַ� ASCII ����
#define MAX_STR_LEN      20    // ����ַ�����

namespace BTV {
	class Canvas {
	public:

		Canvas(HWND);   // ��ʼ������
		~Canvas(void);  // ���ٻ���

		void resize(int, int);           // ���贰�ڴ�С
		void push(int, int = 1, float = 1.0f, float = 1.0f, float = 1.0f, float = 1.0f); // ������Ⱦ�¼�
		void setWaitTime(int);           // ���ö������ʱ��
		void switchStatus(bool);         // �л�����״̬
		void setStatusBar(StatusBar *);  // ����״̬�����
		void unblockOnce(bool = false);  // ȡ������һ��

	private:

		char str[MAX_STR_LEN];              // �����ַ�
		Queue que;                          // �¼�����
		StatusBar *bar;                     // ״̬��
		GLuint font;                        // �ַ���ʾ�б�ʼλ��
		GLYPHMETRICSFLOAT gmf[MAX_CHAR];    // ��¼�ַ���Ϣ
		HDC hDC;                            // �豸������
		HGLRC hRC;                          // ��Դ������
		HWND hWnd;                          // �������ھ��
		std::thread renderThread;           // ��Ⱦ�߳�
		std::mutex mtx;                     // ������
		std::condition_variable cv;         // ��������
		int waitTime;                       // ��ʾ���ʱ��
		volatile bool block;                // ����Ƿ��������
		volatile bool status;               // ������ʾ״̬
		volatile bool loop;                 // ѭ����Ⱦ��־
		volatile bool resized;              // ���ڴ�С�ı��־
		int width, height;                  // �������ڸ߿������ڸ߶�
		float lastTime;                     // ��¼�ϴβ�����ʱ
		GLdouble aspectRatio;               // �����ݺ��
		GLdouble left, right, bottom, top;  // B ��ͼ�δ�Сλ��
		std::chrono::time_point<std::chrono::steady_clock> start;  // ��¼������ʼʱ��
		std::chrono::duration<double, std::milli> duration;        // ����ʱ���

		void init(void);                    // ��ʼ����Ⱦ����
		void render(void);                  // ��Ⱦ����
		void handleResize(void);            // �����ڴ�С�ı�
		void initBar(void);                 // ��ʼ��״̬����ʾ����

	};
}