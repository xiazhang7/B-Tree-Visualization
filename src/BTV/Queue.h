#pragma once

#include <mutex>
#include <condition_variable>

namespace BTV {

	enum queueEvents {   // 定义队列事件
		UNBLOCK_QUEUE,   // 取消队列阻塞一次
		DRAW_NODE,       // 画节点
		DRAW_LINE,       // 画线
		DRAW_FONT,       // 画字符
		CANVAS_INIT,     // 初始化画布
		CANVAS_DONE,     // 渲染图像
		CANVAS_CLEAN,    // 清除画布内容
		STATUS_INSERT,   // 插入键值信息
		STATUS_SEARCH,   // 搜索键值信息
		STATUS_RSEARCH,  // 搜索成功与否信息
		STATUS_DELETE,   // 删除键值信息
		STATUS_PRINT,    // 遍历键值信息
		STATUS_FORK,     // 节点分裂信息
		STATUS_MERGE,    // 节点合并信息
		STATUS_BORROW,   // 节点借值信息
		STATUS_LOAD,     // 文件载入成功与否信息
		STATUS_SAVE      // 文件保存成功与否信息
	};

	class Queue {
	public:

		struct Node { // 队列节点类型
			int type, value;
			float x1, y1, x2, y2;
			Node *next;
		};

		Queue(void);  // 初始化队列
		~Queue(void); // 销毁队列

		void push(int, float, float, float, float, int); // 入队
		Node *pop(void);              // 出队
		void rewind(void);            // 重置当前节点到队列开头
		void end(void);               // 转到最后一个渲染画面
		void destory(Node * = NULL);  // 删除目标节点前的所有节点

	private:

		Node *head, *tail, *now, *last;  // 节点指针
		Node temp;                       // 新建个临时节点用于取消阻塞事件
		std::mutex mtx;                  // 队列线程锁
		std::condition_variable cv;      // 条件变量

	};
}