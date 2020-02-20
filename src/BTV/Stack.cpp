#include "Stack.h"
#include <memory>

using namespace BTV;

template <class T>
Stack<T>::Stack(int size) : Size(size), Top(0) {
	data = (T *)malloc(sizeof(T) * size);    // ����ջ��ʼ�ռ�
	if (!data) throw std::bad_alloc();       // ����ʧ�ܣ��׳��쳣
}

template <class T>
Stack<T>::~Stack(void) {
	free(data);   // ����ջ���ݴ洢��
}

template <class T>
void Stack<T>::push(T element) {
	if (Top == Size) {                                         // ջ��
		T *NewData = (T *)realloc(data, (Size += ADDSIZE));    // ��չջ�ռ�
		if (!NewData) throw std::bad_alloc();                  // ����ʧ�ܣ��׳��쳣
		data = NewData;                                        // �ɹ�������ָ��
	}
	data[Top++] = element;   // ���Ԫ��
}

template <class T>
T Stack<T>::pop(void) {
	return Top ? data[--Top] : -1;    // ����Ԫ�ػ���� -1 ��ջ��λ���½�һ
}

template <class T>
T Stack<T>::top(void) {
	return Top ? data[Top - 1] : -1;  // ����Ԫ�ػ���� -1
}

template <class T>
int Stack<T>::size(void) {
	return Top;  // ����ջ��С
}

template <class T>
void Stack<T>::empty(void) {
	Top = 0;     // ջ��λ������
}