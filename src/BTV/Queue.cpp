#include "Queue.h"

using namespace BTV;

Queue::Queue(void) : head(NULL), now(NULL) {  // ��ʼ��ͷ�͵�ǰ�ڵ�ָ��Ϊ��
	temp.type = UNBLOCK_QUEUE;   // ������ʱ�ڵ��¼�Ϊȡ������
	temp.next = NULL;
}

Queue::~Queue(void) {  // ���ٶ���
	destory();
}

void Queue::push(int type, float x1, float y1, float x2, float y2, int value) {
	Node *node = NULL;
	switch(type) {
	case UNBLOCK_QUEUE:        // ����¼���ȡ������
		if(!now) now = &temp;  // ��������������ǰ�ڵ���Ϊ��ʱ�ڵ�
		break;
	case CANVAS_INIT:
	case CANVAS_CLEAN:
		node = new Node;             // �½��ڵ�
		last = node;                 // ������һ�����濪ͷ
	default:
		if (!node) node = new Node;  // ָ��Ϊ�վ��½��ڵ�
		if (head) tail->next = node; // ��ͷ�ڵ��������β����ڵ�
		else head = node;            // ��������ڵ����ͷ�ڵ�
		node->type = type;           // ��ֵ
		node->x1 = x1;
		node->y1 = y1;
		node->x2 = x2;
		node->y2 = y2;
		node->value = value;
		node->next = NULL;           // �ÿ� next ָ�룬��ʾ�ǽ�β
		tail = node;                 // β�ڵ��Ϊ�½��Ľڵ�
		if (!now) now = node;        // �����ǰ�ڵ�ָ��գ���ǰ�ڵ��л����½��Ľڵ���
		break;
	}
	cv.notify_one();                 // �����߳�
}

Queue::Node *Queue::pop(void) {
	std::unique_lock <std::mutex> lck(mtx);  // ������
	while (!now) cv.wait(lck);               // �ȴ����п��ã��Ӹ� while ��Ϊ�˷�ֹ�������ⱻϵͳ������
	Node *node = now;                        // ȡ����ǰ�ڵ�
	now = now->next;                         // ��ǰ�ڵ�ָ��ָ����һ���ڵ�
	return node;                             // ���ؽڵ�
}

void Queue::rewind(void) {
	now = head;         // ��ǰ�ڵ�ص�ͷ��
	cv.notify_one();    // �����߳�
}

void Queue::end(void) {
	if (now) now = last;  // ת�����һ������
}

void Queue::destory(Node *stop) {
	Node *node;
	while (head != stop) {  // ���ͷ�ڵ㲻��ֹͣ�ڵ�
		node = head;        // �ó�ͷ�ڵ�
		head = head->next;  // ͷ�ڵ�ָ��ָ����һ���ڵ�
		delete node;        // ɾ���ɵ�ͷ�ڵ�
	}
}