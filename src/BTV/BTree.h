#pragma once

#include "Stack.cpp"
#include "Graphics.h"
#include "StatusBar.h"
#include <fstream>
#include <chrono>

#define NODE_SPACE 0.2f            // 图形显示时，节点间的距离
#define CHECK_CODE 201821122083LL  // 二进制文件校验码

namespace BTV {
	class BTree {
	public:

		typedef int KeyType;                    // 定义键值类型

		BTree(int = 3);                         // 初始化 B 树
		~BTree(void);                           // 销毁 B 树

		bool Insert(KeyType);                   // 插入值
		bool Search(KeyType);                   // 搜索值
		bool Delete(KeyType);                   // 删除值
		void Print(bool);                       // 打印 B 树
		void Destory(void);                     // 摧毁整棵树

		bool SetDegree(int);                    // 设置 B 树阶数

		bool Open(std::fstream &, bool);        // 打开文件
		bool Save(std::fstream &);              // 保存文件

		void SetCanvas(Canvas *canvas)          { this->canvas = canvas; } // 设置画布
		void SetStatusBar(StatusBar *);         // 设置状态栏句柄

	private:

		StatusBar *bar;                         // 储存状态栏指针
		int Degree, MinDegree, MaxDegree;       // B 树的阶数、最大键值数、最小键值数
		Stack<int> IdStack, TStack;             // 储存搜索路径( B 树操作与遍历 )
		Canvas *canvas;                         // 储存画布类指针
		float TreeWidth;                        // 记录树的图形宽度
		bool InitialDraw;                       // 记录是否绘制第一个节点
		std::chrono::time_point<std::chrono::steady_clock> startTime;  // 记录动作开始时间
		std::chrono::duration<double, std::milli> duration;            // 保存时间差
		struct Node {
			int KeyNum;                         // 节点值的数量
			KeyType *Keys;                      // 节点含的值
			Node *Parent, **Children;           // 节点的父亲和孩子们
		}*Tree, *nn;

		// 内部 B 树操作函数
		Node    *NewNode(std::fstream * = NULL, Node * = NULL);                // 新建一个节点
		bool    DelNode(Node *);                                               // 删除一个节点，返回成功与否
		int     NodeSearch(Node *, KeyType);                                   // 在节点中寻找指定值，返回位置
		Node    *TreeSearch(KeyType, int * = NULL);                            // 在整棵 B 树中寻找指定值，返回所在节点
		void    NodeInsert(Node *, KeyType, int, Node * = NULL, bool = true);  // 在节点的指定位置插入指定值
		KeyType NodeDelete(Node *, int, Node ** = NULL, bool = true);          // 删除节点中指定位置上的值，返回删除的值
		Node    *FindMaxLeaf(Node *);           // 寻找比指定节点最小值小的最大值所在节点
		bool    BorrowKey(Node *, int);         // 向左或右兄弟借一个值，返回成功与否
		void    Fork(Node *);                   // 分裂节点为两个小的节点
		void    Merge(Node *, int);             // 合并一个合适的相邻节点
		template<typename ...Ts>
		bool    TraversalTree(bool (BTree::*CallBack)(Node *, Ts ...args), bool, bool = true, Ts ...args); // 遍历树，每个节点传给回调函数处理

		// 文件操作函数
		bool    HandleTextFile(std::fstream &);            // 处理文本文件
		bool    HandleBinaryFile(std::fstream &);          // 处理二进制文件
		bool    IsSameString(const char *, const char *);  // 判断字符串是否相等（不区分大小写）

		// B 树遍历回调函数
		bool    ReadFileCallBack(Node *, std::fstream *);  // 读文件到节点
		bool    SaveFileCallBack(Node *, std::fstream *);  // 保存节点为文件
		bool    PrintTreeCallBack(Node *);                 // 遍历打印节点键值
		bool    DrawTreeCallBack(Node *, Node *, int);     // 节点绘制函数

		// 图像渲染函数
		void    DrawTree(Node * = NULL, int = -1, double = -1.0f);  // 绘制并渲染 B 树

		// 状态栏阶数显示函数
		void    ShowDegree(void);
	};
}