#pragma once

#include "Stack.cpp"
#include "Graphics.h"
#include "StatusBar.h"
#include <fstream>
#include <chrono>

#define NODE_SPACE 0.2f            // ͼ����ʾʱ���ڵ��ľ���
#define CHECK_CODE 201821122083LL  // �������ļ�У����

namespace BTV {
	class BTree {
	public:

		typedef int KeyType;                    // �����ֵ����

		BTree(int = 3);                         // ��ʼ�� B ��
		~BTree(void);                           // ���� B ��

		bool Insert(KeyType);                   // ����ֵ
		bool Search(KeyType);                   // ����ֵ
		bool Delete(KeyType);                   // ɾ��ֵ
		void Print(bool);                       // ��ӡ B ��
		void Destory(void);                     // �ݻ�������

		bool SetDegree(int);                    // ���� B ������

		bool Open(std::fstream &, bool);        // ���ļ�
		bool Save(std::fstream &);              // �����ļ�

		void SetCanvas(Canvas *canvas)          { this->canvas = canvas; } // ���û���
		void SetStatusBar(StatusBar *);         // ����״̬�����

	private:

		StatusBar *bar;                         // ����״̬��ָ��
		int Degree, MinDegree, MaxDegree;       // B ���Ľ���������ֵ������С��ֵ��
		Stack<int> IdStack, TStack;             // ��������·��( B ����������� )
		Canvas *canvas;                         // ���滭����ָ��
		float TreeWidth;                        // ��¼����ͼ�ο��
		bool InitialDraw;                       // ��¼�Ƿ���Ƶ�һ���ڵ�
		std::chrono::time_point<std::chrono::steady_clock> startTime;  // ��¼������ʼʱ��
		std::chrono::duration<double, std::milli> duration;            // ����ʱ���
		struct Node {
			int KeyNum;                         // �ڵ�ֵ������
			KeyType *Keys;                      // �ڵ㺬��ֵ
			Node *Parent, **Children;           // �ڵ�ĸ��׺ͺ�����
		}*Tree, *nn;

		// �ڲ� B ����������
		Node    *NewNode(std::fstream * = NULL, Node * = NULL);                // �½�һ���ڵ�
		bool    DelNode(Node *);                                               // ɾ��һ���ڵ㣬���سɹ����
		int     NodeSearch(Node *, KeyType);                                   // �ڽڵ���Ѱ��ָ��ֵ������λ��
		Node    *TreeSearch(KeyType, int * = NULL);                            // ������ B ����Ѱ��ָ��ֵ���������ڽڵ�
		void    NodeInsert(Node *, KeyType, int, Node * = NULL, bool = true);  // �ڽڵ��ָ��λ�ò���ָ��ֵ
		KeyType NodeDelete(Node *, int, Node ** = NULL, bool = true);          // ɾ���ڵ���ָ��λ���ϵ�ֵ������ɾ����ֵ
		Node    *FindMaxLeaf(Node *);           // Ѱ�ұ�ָ���ڵ���СֵС�����ֵ���ڽڵ�
		bool    BorrowKey(Node *, int);         // ��������ֵܽ�һ��ֵ�����سɹ����
		void    Fork(Node *);                   // ���ѽڵ�Ϊ����С�Ľڵ�
		void    Merge(Node *, int);             // �ϲ�һ�����ʵ����ڽڵ�
		template<typename ...Ts>
		bool    TraversalTree(bool (BTree::*CallBack)(Node *, Ts ...args), bool, bool = true, Ts ...args); // ��������ÿ���ڵ㴫���ص���������

		// �ļ���������
		bool    HandleTextFile(std::fstream &);            // �����ı��ļ�
		bool    HandleBinaryFile(std::fstream &);          // ����������ļ�
		bool    IsSameString(const char *, const char *);  // �ж��ַ����Ƿ���ȣ������ִ�Сд��

		// B �������ص�����
		bool    ReadFileCallBack(Node *, std::fstream *);  // ���ļ����ڵ�
		bool    SaveFileCallBack(Node *, std::fstream *);  // ����ڵ�Ϊ�ļ�
		bool    PrintTreeCallBack(Node *);                 // ������ӡ�ڵ��ֵ
		bool    DrawTreeCallBack(Node *, Node *, int);     // �ڵ���ƺ���

		// ͼ����Ⱦ����
		void    DrawTree(Node * = NULL, int = -1, double = -1.0f);  // ���Ʋ���Ⱦ B ��

		// ״̬��������ʾ����
		void    ShowDegree(void);
	};
}