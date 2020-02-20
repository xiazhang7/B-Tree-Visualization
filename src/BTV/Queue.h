#pragma once

#include <mutex>
#include <condition_variable>

namespace BTV {

	enum queueEvents {   // ��������¼�
		UNBLOCK_QUEUE,   // ȡ����������һ��
		DRAW_NODE,       // ���ڵ�
		DRAW_LINE,       // ����
		DRAW_FONT,       // ���ַ�
		CANVAS_INIT,     // ��ʼ������
		CANVAS_DONE,     // ��Ⱦͼ��
		CANVAS_CLEAN,    // �����������
		STATUS_INSERT,   // �����ֵ��Ϣ
		STATUS_SEARCH,   // ������ֵ��Ϣ
		STATUS_RSEARCH,  // �����ɹ������Ϣ
		STATUS_DELETE,   // ɾ����ֵ��Ϣ
		STATUS_PRINT,    // ������ֵ��Ϣ
		STATUS_FORK,     // �ڵ������Ϣ
		STATUS_MERGE,    // �ڵ�ϲ���Ϣ
		STATUS_BORROW,   // �ڵ��ֵ��Ϣ
		STATUS_LOAD,     // �ļ�����ɹ������Ϣ
		STATUS_SAVE      // �ļ�����ɹ������Ϣ
	};

	class Queue {
	public:

		struct Node { // ���нڵ�����
			int type, value;
			float x1, y1, x2, y2;
			Node *next;
		};

		Queue(void);  // ��ʼ������
		~Queue(void); // ���ٶ���

		void push(int, float, float, float, float, int); // ���
		Node *pop(void);              // ����
		void rewind(void);            // ���õ�ǰ�ڵ㵽���п�ͷ
		void end(void);               // ת�����һ����Ⱦ����
		void destory(Node * = NULL);  // ɾ��Ŀ��ڵ�ǰ�����нڵ�

	private:

		Node *head, *tail, *now, *last;  // �ڵ�ָ��
		Node temp;                       // �½�����ʱ�ڵ�����ȡ�������¼�
		std::mutex mtx;                  // �����߳���
		std::condition_variable cv;      // ��������

	};
}