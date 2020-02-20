#include "Stack.h"
#include <memory>

using namespace BTV;

template <class T>
Stack<T>::Stack(int size) : Size(size), Top(0) {
	data = (T *)malloc(sizeof(T) * size);    // 分配栈初始空间
	if (!data) throw std::bad_alloc();       // 分配失败，抛出异常
}

template <class T>
Stack<T>::~Stack(void) {
	free(data);   // 销毁栈数据存储区
}

template <class T>
void Stack<T>::push(T element) {
	if (Top == Size) {                                         // 栈满
		T *NewData = (T *)realloc(data, (Size += ADDSIZE));    // 扩展栈空间
		if (!NewData) throw std::bad_alloc();                  // 分配失败，抛出异常
		data = NewData;                                        // 成功，设置指针
	}
	data[Top++] = element;   // 添加元素
}

template <class T>
T Stack<T>::pop(void) {
	return Top ? data[--Top] : -1;    // 返回元素或错误 -1 ，栈顶位置下降一
}

template <class T>
T Stack<T>::top(void) {
	return Top ? data[Top - 1] : -1;  // 返回元素或错误 -1
}

template <class T>
int Stack<T>::size(void) {
	return Top;  // 返回栈大小
}

template <class T>
void Stack<T>::empty(void) {
	Top = 0;     // 栈顶位置置零
}