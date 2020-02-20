#include "Queue.h"

using namespace BTV;

Queue::Queue(void) : head(NULL), now(NULL) {  // 初始化头和当前节点指针为空
	temp.type = UNBLOCK_QUEUE;   // 设置临时节点事件为取消阻塞
	temp.next = NULL;
}

Queue::~Queue(void) {  // 销毁队列
	destory();
}

void Queue::push(int type, float x1, float y1, float x2, float y2, int value) {
	Node *node = NULL;
	switch(type) {
	case UNBLOCK_QUEUE:        // 如果事件是取消阻塞
		if(!now) now = &temp;  // 若队列阻塞，当前节点设为临时节点
		break;
	case CANVAS_INIT:
	case CANVAS_CLEAN:
		node = new Node;             // 新建节点
		last = node;                 // 标记最后一幅画面开头
	default:
		if (!node) node = new Node;  // 指针为空就新建节点
		if (head) tail->next = node; // 有头节点就往链表尾部插节点
		else head = node;            // 否则这个节点就是头节点
		node->type = type;           // 赋值
		node->x1 = x1;
		node->y1 = y1;
		node->x2 = x2;
		node->y2 = y2;
		node->value = value;
		node->next = NULL;           // 置空 next 指针，表示是结尾
		tail = node;                 // 尾节点变为新建的节点
		if (!now) now = node;        // 如果当前节点指针空，当前节点切换到新建的节点上
		break;
	}
	cv.notify_one();                 // 解锁线程
}

Queue::Node *Queue::pop(void) {
	std::unique_lock <std::mutex> lck(mtx);  // 创建锁
	while (!now) cv.wait(lck);               // 等待队列可用（加个 while 是为了防止进程意外被系统解锁）
	Node *node = now;                        // 取出当前节点
	now = now->next;                         // 当前节点指针指向下一个节点
	return node;                             // 返回节点
}

void Queue::rewind(void) {
	now = head;         // 当前节点回到头部
	cv.notify_one();    // 解锁线程
}

void Queue::end(void) {
	if (now) now = last;  // 转到最后一个画面
}

void Queue::destory(Node *stop) {
	Node *node;
	while (head != stop) {  // 如果头节点不是停止节点
		node = head;        // 拿出头节点
		head = head->next;  // 头节点指针指向下一个节点
		delete node;        // 删除旧的头节点
	}
}