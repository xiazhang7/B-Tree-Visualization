#pragma once

#define ADDSIZE 16   // 栈的空间递增大小

namespace BTV {
	template <class T>
	class Stack {
	public:

		Stack(int = ADDSIZE);  // 初始化栈
		~Stack(void);          // 销毁栈

		void push(T);          // 入栈
		T    pop(void);        // 出栈
		T    top(void);        // 获取栈顶元素
		int  size(void);       // 获得元素个数
		void empty(void);      // 清空栈

	private:

		int Top, Size;         // 栈顶位置、栈大小\当前位置
		T *data;               // 栈数据储存在这

	};
}