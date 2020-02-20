#pragma once

#include "File.h"
#include "BTree.h"
#include "Graphics.h"
#include "StatusBar.h"
#include <Windows.h>

#define ANIMATION_PAUSE 0  // ������ͣ
#define ANIMATION_START 1  // ������ʼ
#define ANIMATION_STOP  2  // ��������

namespace BTV {

	class Controller {
	public:

		Controller(BTree *);               // ���캯��

		void setMainHandle(HWND hWnd)      { mainHandle = hWnd; }           // ���������ھ��
		void setCanvasHandle(HWND hWnd)    { canvasHandle = hWnd; }         // ���û������ھ��
		void setToolBarHandle(HWND hWnd)   { toolBarHandle = hWnd; }        // ���ö�����������ť
		void setStatusBarHandle(HWND hWnd) { statusBarHandle = hWnd; }      // ����״̬�����
		void setStatusBar(StatusBar *bar)  { this->bar = bar; }             // ����״̬��
		void setCanvas(Canvas *canvas)     { this->canvas = canvas; }       // ���û���
		bool paint(HWND hWnd)              { return hWnd != canvasHandle; } // ���û������ڲ��Զ��ػ�
		void step(void)                    { canvas->unblockOnce(); }       // ������ʾһ������
		void setDialogHandle(HWND);        // ���ÿ��ƴ��ھ��
		void open(const wchar_t * = NULL); // ���ļ�
		void save(void);                   // �����ļ�
		void drag(HDROP);                  // ������ק�ļ�
		void command(int);                 // ����ť�¼�
		void resize(HWND, int, int);       // ��Ӧ���ڴ�С
		void speed(void);                  // ���ö����ٶ�
		void animation(int);               // �л�����״̬

	private:

		HWND mainHandle, canvasHandle, dialogHandle, sliderHandle, toolBarHandle, statusBarHandle;  // ���ھ��
		Canvas *canvas;      // ����
		BTree *tree;         // B ����
		File file;           // �ļ���
		StatusBar *bar;      // ״̬����
		std::fstream fstm;   // �ļ���
		wchar_t s[MAX_PATH]; // ��ʱ�����ַ���

		bool handleCommand(int &, int);  // ����ť�¼�
		void setToolBarButton(bool);     // ���ò˵�����ť״̬

	};
}