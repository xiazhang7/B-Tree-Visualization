#pragma once

#define ADDSIZE 16   // ջ�Ŀռ������С

namespace BTV {
	template <class T>
	class Stack {
	public:

		Stack(int = ADDSIZE);  // ��ʼ��ջ
		~Stack(void);          // ����ջ

		void push(T);          // ��ջ
		T    pop(void);        // ��ջ
		T    top(void);        // ��ȡջ��Ԫ��
		int  size(void);       // ���Ԫ�ظ���
		void empty(void);      // ���ջ

	private:

		int Top, Size;         // ջ��λ�á�ջ��С\��ǰλ��
		T *data;               // ջ���ݴ�������

	};
}