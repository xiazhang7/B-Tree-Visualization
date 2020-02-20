#include "BTree.h"

using namespace BTV;

// B 树基本操作部分函数开始
BTree::BTree(int Degree) : Degree(0), Tree(NULL), TreeWidth(0.0f), bar(NULL) { // 初始化阶为 0, 树为空，图形宽度为 0, 状态栏指针为空
	SetDegree(Degree);  // 设置阶数
}

BTree::~BTree(void) {
	Destory();          // 删除类时销毁 B 树
}

bool BTree::SetDegree(int Degree) {
	if (Degree < 3) return false;     // 阶数最小为 3
	if (Degree != this->Degree) {     // 如果阶数不一样
		if (this->Degree) Destory();  // 并且不是初值，摧毁树
	} else return true;               // 否则不用操作了
	this->Degree = Degree;            // 设置阶数
	MaxDegree = Degree - 1;           // 设置最大键值数
	MinDegree = MaxDegree / 2;        // 设置最小键值数
	ShowDegree();                     // 状态栏显示当前阶数
	return true;
}

BTree::Node *BTree::NewNode(std::fstream *fstm, Node *node) {
	if (!node) {
		node = new Node;                                  // 分配新节点
		node->Keys = new KeyType[Degree];                 // 分配键值储存空间
		node->Children = new Node*[Degree + 1]();         // 分配孩子指针储存空间并填零
	}
	if (fstm) {                                           // 如果有文件流，从文件流初始化
		fstm->read((char *)&node->KeyNum, sizeof(int));                    // 键值数
		fstm->read((char *)node->Keys, sizeof(KeyType) * Degree);          // 键值
		fstm->read((char *)node->Children, sizeof(Node *) * (Degree + 1)); // 孩子指针
	}
	return node;
}

bool BTree::DelNode(Node *node) {
	if (!node) return false;    // 空指针，返回失败
	delete [](node->Keys);      // 删除键值储存空间
	delete [](node->Children);  // 删除孩子指针储存空间
	delete node;                // 删除节点
	return true;
}

void BTree::Print(bool parentFirst) {
	canvas->push(STATUS_PRINT, parentFirst);                       // 显示当前操作
	duration = std::chrono::microseconds::zero();                  // 操作时间清零
	startTime = std::chrono::steady_clock::now();                  // 记录开始时间
	TraversalTree(&BTree::PrintTreeCallBack, parentFirst, false);  // 遍历打印 B 树
	duration += std::chrono::steady_clock::now() - startTime;      // 计算时间差
	DrawTree(NULL, -1, duration.count());                          // 打印最终状态
}

bool BTree::PrintTreeCallBack(Node *node) {
	for(int i = 0; i < node->KeyNum; i++) DrawTree(node, i);       // 渲染当前节点所有键值
	return true;
}

void BTree::Destory(void) {
	TraversalTree(&BTree::DelNode, false);  // 遍历删除每个节点
	Tree = NULL;                            // 树根指针置空
	TreeWidth = 0.0f;                       // 重置图形宽度
	DrawTree();                             // 显示结果（清空画布）
}

template<typename ...Ts>
bool BTree::TraversalTree(bool (BTree::*CallBack)(Node *, Ts ...args), bool parentFirst, bool useTStack, Ts ...args) {
	if (!Tree) return true;               // 空树，不用遍历
	Node *node = Tree, *Parent;           // 从树根开始遍历
	Stack<int> *stack;
	if (useTStack) stack = &TStack;       // 选择使用的路径栈
	else stack = &IdStack;
	if (parentFirst)
		if (!(this->*CallBack)(node, std::forward<Ts>(args)...))
			return false;                 // 先序，先对父节点操作，操作失败则终止
	int id = 0;                           // 从最左端节点开始遍历
	stack->empty();                       // 清空路径栈
	while (true) {
		if (node->Children[id]) {         // 有孩子，继续往下遍历
			node = node->Children[id];    // 往子节点走
			stack->push(id);              // 记录当前路径
			if(parentFirst)
				if (!(this->*CallBack)(node, std::forward<Ts>(args)...))
					return false;         // 先序，先对该节点操作，操作失败则终止
			id = 0;                       // 子节点从最左端节点开始遍历
		} else {                          // 没有孩子，说明是叶节点，准备返回父节点
			do {
				Parent = node->Parent;    // 先保存父节点，防止节点被删找不到父亲
				if(!parentFirst)
					if(!(this->*CallBack)(node, std::forward<Ts>(args)...))
						return false;     // 后序，没有孩子准备返回父节点时操作该节点，操作失败则终止
				id = stack->pop() + 1;    // 返回父节点后遍历下一个子节点
				if (!id) return true;     // 如果 id + 1 等于 0, 说明栈空，遍历结束
				node = Parent;            // 返回父节点
			} while (id > node->KeyNum);  // 如果越界就继续往父节点走
		}
	}
	return true;
}

void BTree::ShowDegree(void) {
	if (bar) bar->msg(DEGREE_STATUSBAR, L"阶数：%d", Degree);  // 显示初始
}

void BTree::SetStatusBar(StatusBar *bar) {
	this->bar = bar;  // 设置状态栏句柄
	ShowDegree();     // 显示阶数
}
// B 树基本操作部分函数结束

// B 树插入部分函数开始
bool BTree::Insert(KeyType Key) {
	if (Degree < 3) return false;      // 阶未设置或太小
	canvas->push(STATUS_INSERT, Key);  // 显示当前操作
	duration = std::chrono::microseconds::zero();  // 操作时间清零
	startTime = std::chrono::steady_clock::now();  //记录开始时间
	if (!Tree) {                       // 如果树是空的
		Tree = NewNode();              // 分配新节点
		Tree->KeyNum = 0;              // 节点值数为 0
		Tree->Parent = NULL;           // 根节点，没有父亲
	}
	Node *node = Tree;
	int i = NodeSearch(node, Key);     // 查找元素该存放的位置
	IdStack.empty();                   // 清空路径栈
	while (node->Children[0]) {        // 不是叶子节点就一直往下
		IdStack.push(i);               // 记录路径
		node = node->Children[i];      // 往下一个节点走
		i = NodeSearch(node, Key);     // 查找元素该存放的位置
	}
	NodeInsert(node, Key, i);          // 找到节点，插入值
	duration += std::chrono::steady_clock::now() - startTime;  // 计算时间差
	DrawTree(NULL, -1, duration.count());                      // 打印最终状态
	return true;
}

int BTree::NodeSearch(Node *node, KeyType Key) {
	int i;
	for (i = 0; i < node->KeyNum && Key > node->Keys[i]; i++)   // 查找指定位置（键值）并渲染过程
		DrawTree(node, i);
	if (i < node->KeyNum && Key == node->Keys[i])  // 如果找到目标键值
		DrawTree(node, i);        // 节点、键值标红
	else if (!i && node->KeyNum)  // 如果 i 为零并且节点不是空节点
		DrawTree(node);           // 节点标红
	return i;
}

void BTree::NodeInsert(Node *node, KeyType Key, int i, Node *child, bool rightChild) {
	int end;
	for (end = node->KeyNum; end > i; end--)
		node->Keys[end] = node->Keys[end - 1];               // 每个节点后移一位
	node->Keys[i] = Key;                                     // 指定位置插入值
	node->KeyNum++;                                          // 节点含有键值数 +1
	if (child) {                                             // 如果参数带有孩子指针
		if (rightChild) i++;                                 // 如果操作右孩子，末位加一
		for (end = node->KeyNum; end > i; end--)
			node->Children[end] = node->Children[end - 1];   // 每个孩子指针后移一位
		node->Children[end] = child;                         // 插入这个键值的右孩子
	}
	if(!node->Children[0]) TreeWidth += 1.0f;                // 如果在叶子节点上插入键值，图形宽度加一
	if (node->KeyNum == Degree) {
		canvas->push(STATUS_FORK, Degree);   // 状态栏显示节点分裂
		DrawTree(node);                      // 分裂之前渲染一下树的状态
		Fork(node);                          // 如果插入后节点满了就分裂
	}
}

void BTree::Fork(Node *node1) {
	int i, j;
	Node *node2 = NewNode();                                 // 分配空间给分裂的新节点
	bool hasChild = node1->Children[0] ? true : false;       // 判断节点是否有孩子
	for (i = MinDegree + 1, j = 0; i < Degree; i++, j++) {   // 移动原节点后半部分到新节点
		node2->Keys[j] = node1->Keys[i];                     // 移动键值
		if (hasChild) {                                      // 如果有孩子
			node2->Children[j] = node1->Children[i];         // 移动孩子指针
			node1->Children[i] = NULL;                       // 置空原位置的指针
			node2->Children[j]->Parent = node2;              // 更新子节点的父亲为新节点
		}
	}
	if (hasChild) {
		node2->Children[j] = node1->Children[i];  // 剩下一个孩子指针单独移动
		node1->Children[i] = NULL;                // 单独置空
		node2->Children[j]->Parent = node2;       // 单独更新父指针
	} else TreeWidth -= 1.0f - NODE_SPACE;        // 叶子节点分裂，图形宽度减少
	node1->KeyNum = MinDegree;                    // 设置原节点键值数
	node2->KeyNum = MaxDegree - MinDegree;        // 设置新节点键值数
	if (node1->Parent) {                          // 如果不是根节点
		node2->Parent = node1->Parent;            // 新节点的父亲就是原节点的父亲
		NodeInsert(                               // 中间元素向上插入父节点中
			node1->Parent,
			node1->Keys[MinDegree],
			IdStack.pop(),
			node2
		);
	} else {                                      // 如果是根节点
		Tree = NewNode();                         // 新建一个根节点
		Tree->Parent = NULL;                      // 根节点没有父亲
		node1->Parent = node2->Parent = Tree;     // 分裂出来的两个节点的父亲都是根节点
		Tree->Keys[0] = node1->Keys[MinDegree];   // 根节点的第一个元素的原节点的中间节点
		Tree->Children[0] = node1;                // 第一个元素的左孩子是原节点
		Tree->Children[1] = node2;                // 第一个元素的右孩子是新节点
		Tree->KeyNum = 1;                         // 根节点现在只有一个元素
	}
}
// B 树插入部分函数结束

// B 树搜索部分函数开始
BTree::Node *BTree::TreeSearch(KeyType Key, int *id) {
	int i;
	IdStack.empty();                   // 清空路径栈
	for (Node *node = Tree; node; node = node->Children[i]) {
		i = NodeSearch(node, Key);     // 寻找该值在节点中应该待的位置
		if (i < node->KeyNum && node->Keys[i] == Key) {    // 如果找到了
			canvas->push(STATUS_RSEARCH, 1);  // 状态栏显示找到
			if(id) *id = i;            // 记录值在节点中的位置
			return node;               // 返回值所在节点
		}
		IdStack.push(i);               // 记录路径
	}
	canvas->push(STATUS_RSEARCH, 0);   // 状态栏显示未找到
	return NULL;      // 找不到，返回空指针
}

bool BTree::Search(KeyType Key) {
	canvas->push(STATUS_SEARCH, Key);                          // 显示当前操作
	duration = std::chrono::microseconds::zero();              // 操作时间清零
	startTime = std::chrono::steady_clock::now();              // 记录开始时间
	bool result = TreeSearch(Key);                             // 搜索节点
	duration += std::chrono::steady_clock::now() - startTime;  // 计算时间差
	DrawTree(NULL, -1, duration.count());                      // 打印最终状态
	return result;
}
// B 树搜索部分函数结束

// B 树删除部分函数开始
bool BTree::Delete(KeyType Key) {
	int id;
	canvas->push(STATUS_DELETE, Key);                              // 显示当前操作
	duration = std::chrono::microseconds::zero();                  // 操作时间清零
	startTime = std::chrono::steady_clock::now();                  // 记录开始时间
	Node *node = TreeSearch(Key, &id);                             // 在树中找到当前值所在节点
	if (!node) {
		duration += std::chrono::steady_clock::now() - startTime;  // 计算时间差
		DrawTree(NULL, -1, duration.count());                      // 删除失败，渲染树的状态
		return false;                                              // 找不到节点则删除失败
	}
	if (node->Children[0]) {                                       // 如果这个节点不是叶子节点
		IdStack.push(id);                                          // 推送此节点位置到路径栈中
		Node *LeafNode = FindMaxLeaf(node->Children[id]);          // 找到比当前值小的最大值所在叶子节点
		node->Keys[id] = LeafNode->Keys[LeafNode->KeyNum - 1];     // 把这个最大值赋给当前键
		node = LeafNode, id = LeafNode->KeyNum - 1;                // 操作节点变成叶子节点
	}
	NodeDelete(node, id);   // 删除对应的值
	while (node->KeyNum < MinDegree && node->Parent) {  // 当键值数小于最小要求并且这个节点不是根节点时
		id = IdStack.pop();                         // 找到此节点上一个节点的孩子位置
		if (BorrowKey(node, id)) {                  // 找兄弟借到值,万事大吉
			break;                                  // 跳出判断，因为父节点键值数不变
		} else {                                    // 借不到值
			canvas->push(STATUS_MERGE, MinDegree);  // 状态栏显示合并信息
			Node *Parent = node->Parent;            // 保存父节点，防止当前节点被删找不到父亲
			Merge(node, id);                        // 找个兄弟节点合并
			node = Parent;                          // 往父节点走，看看父节点是不是满足要求
			DrawTree(node);                         // 合并完成，渲染树的状态
		}
	}
	if (node == Tree && !node->KeyNum) {   // 如果这个节点是根节点并且键值数为 0
		if (Tree->Children[0]) {
			Tree = Tree->Children[0];      // 如果根节点有孩子，孩子变成根节点
			Tree->Parent = NULL;           // 根节点没有父亲
		} else Tree = NULL;                // 否则整棵树就空了
		DelNode(node);                     // 删除这个节点
	}
	duration += std::chrono::steady_clock::now() - startTime;  // 计算时间差
	DrawTree(NULL, -1, duration.count());                      // 删除完成，渲染树的状态
	return true;
}

BTree::KeyType BTree::NodeDelete(Node *node, int i, Node **child, bool rightChild) {
	KeyType Key = node->Keys[i];                           // 保存要删除键值
	int id;
	for (id = i; id < node->KeyNum - 1; id++)
		node->Keys[id] = node->Keys[id + 1];               // 循环向前移动键值（删除键值）
	if (node->Children[0]) {                               // 如果有孩子，删掉对应孩子指针
		id = rightChild ? i + 1 : i;                       // 判断要操作左孩子还是右孩子
		if (child) *child = node->Children[id];            // 保存删除的孩子（如果需要）
		for (; id < node->KeyNum; id++)
			node->Children[id] = node->Children[id + 1];   // 循环向前移动孩子指针（删除孩子指针）
	} else TreeWidth -= 1.0f;                              // 删除键值，图形宽度减一
	node->Children[id] = NULL;   // 最后一个指针置空
	node->KeyNum--;              // 键值数减一
	return Key;                  // 返回被删的键值
}

BTree::Node *BTree::FindMaxLeaf(Node *node) {
	while (node->Children[0]) {               // 当节点有孩子
		DrawTree(node);                       // 渲染查找最大键值的过程
		IdStack.push(node->KeyNum);           // 记录路径
		node = node->Children[node->KeyNum];  // 往最大孩子方向走
	}
	DrawTree(node, node->KeyNum - 1);         // 渲染查找到最大键值的状态
	return node;
}

bool BTree::BorrowKey(Node *node, int id) {
	Node *Parent = node->Parent, *Target, *Temp = NULL;
	KeyType Key;
	if (id && Parent->Children[id - 1]->KeyNum > MinDegree) {  // 向左兄弟借值
		canvas->push(STATUS_BORROW, 0);                                   // 状态栏显示借值信息
		id--;                                                             // 操作左兄弟
		Target = Parent->Children[id];                                    // 目标是左兄弟
		Key = Parent->Keys[id];                                           // 保存父键值
		Parent->Keys[id] = NodeDelete(Target, Target->KeyNum - 1, &Temp); // 左兄弟的最键值给父亲键，保存最右孩子指针
		NodeInsert(node, Key, 0, Temp, false);                            // 把父键值和孩子指针插入当前节点开头
	} else if (id != Parent->KeyNum && Parent->Children[id + 1]->KeyNum > MinDegree) {  // 向右兄弟借值,步骤类同
		canvas->push(STATUS_BORROW, 1);                                   // 状态栏显示借值信息
		id++;                                                             // 操作右兄弟
		Target = Parent->Children[id];                                    // 目标是右兄弟
		Key = Parent->Keys[id - 1];                                       // 保存父键值
		Parent->Keys[id - 1] = NodeDelete(Target, 0, &Temp, false);       // 右兄弟的最键值给父亲键，保存最左孩子指针
		NodeInsert(node, Key, node->KeyNum, Temp);                        // 把父键值和孩子指针插入当前节点结尾
	} else return false;            // 无法向左右兄弟借值，返回失败
	if(Temp) Temp->Parent = node;   // 修改孩子指针的父亲为当前节点
	return true;                    // 成功借到值
}

void BTree::Merge(Node *node, int id) {
	Node *Target;
	int j = 0;
	if (id) Target = node->Parent->Children[--id];                  // 当前节点不是第一个节点，目标节点设为左兄弟
	else Target = node, node = node->Parent->Children[id + 1];      // 是第一个节点，目标节点设为当前节点，当前节点设为右兄弟
	Target->Keys[Target->KeyNum] = NodeDelete(Target->Parent, id);  // 父键值追加到目标节点
	bool hasChild = node->Children[0] ? true : false;               // 判断当前节点是否有孩子
	for (id = Target->KeyNum + 1; j < node->KeyNum; id++, j++) {    // 循环遍历当前节点
		Target->Keys[id] = node->Keys[j];                           // 键值追加到目标节点
		if (hasChild) {
			Target->Children[id] = node->Children[j];               // 有孩子，孩子指针追加到目标节点
			Target->Children[id]->Parent = Target;                  // 修改孩子节点的父亲为目标节点
		}
	}
	if (hasChild) {
		Target->Children[id] = node->Children[j]; // 剩下一个指针单独追加
		Target->Children[id]->Parent = Target;    // 单独修改父亲指针
	} else TreeWidth += 1.0f - NODE_SPACE;        // 合并节点，图形宽度增加
	Target->KeyNum = id;  // 设置目标节点的键值个数
	DelNode(node);        // 删除当前节点
}
// B 树删除部分函数结束

// B 树文件操作部分函数开始
bool BTree::Open(std::fstream &fstm, bool isBinary) {
	bool result;
	if (isBinary) {
		duration = std::chrono::microseconds::zero();              // 操作时间清零
		startTime = std::chrono::steady_clock::now();              // 记录开始时间
		result = HandleBinaryFile(fstm);                           // 二进制文件操作
		duration += std::chrono::steady_clock::now() - startTime;  // 计算时间差
		DrawTree(NULL, -1, duration.count());                      // 读完绘制 B 树
	} else result = HandleTextFile(fstm);    // 文本文件操作
	fstm.close();                            // 关闭文件流
	return result;                           // 返回结果
}

bool BTree::Save(std::fstream &fstm) {
	const long long checkHeader = CHECK_CODE;
	fstm.write((char *)&checkHeader, sizeof(long long));  // 写入校验头
	fstm.write((char *)&Degree, sizeof(int));             // 写入阶数
	fstm.write((char *)&TreeWidth, sizeof(float));        // 写入图形宽度
	if (fstm.fail()) return false;                        // 写入失败
	bool result = TraversalTree(&BTree::SaveFileCallBack, true, true, &fstm);  // 递归写入所有节点
	fstm.close();                // 关闭文件流
	if (!result) return false;   // 递归写入时错误
	return true;
}

bool BTree::HandleTextFile(std::fstream &fstm) {
	char headStr[7];                                                 // 储存前半部分字符串
	fstm >> headStr;                                                 // 读入阶数字符串
	if (!IsSameString(headStr, "degree")) return false;              // 判断字符串是不是"degree"，起校验作用
	int tailNum;                                                     // 储存后半部分数字
	if (!(fstm >> tailNum) || !SetDegree(tailNum)) return false;     // 无法读入阶数或阶数不对则失败终止
	while (fstm >> headStr >> tailNum) {                             // 循环读取操作和值
		if (IsSameString(headStr, "insert")) Insert(tailNum);        // 插入
		else if (IsSameString(headStr, "delete")) Delete(tailNum);   // 删除
		else if (IsSameString(headStr, "search")) Search(tailNum);   // 查找
		else if (IsSameString(headStr, "print")) Print(tailNum);     // 打印
		else return false;                                           // 找不到相应功能，执行失败
	}
	return true;
}

bool BTree::HandleBinaryFile(std::fstream &fstm) {
	long long checkHeader;
	fstm.read((char *)&checkHeader, sizeof(long long));  // 读入文件校验头
	if (checkHeader != CHECK_CODE) return false;         // 文件校验失败
	if (Tree) Destory();                                 // 如果树不是空的，先摧毁
	int Degree;
	fstm.read((char *)&Degree, sizeof(int));       // 读阶数
	if(!SetDegree(Degree)) return false;           // 设置阶数
	fstm.read((char *)&TreeWidth, sizeof(float));  // 读入图形宽度
	Tree = NewNode(&fstm);                         // 新建一个根节点并从文件流初始化
	Tree->Parent = NULL;                           // 根节点无父亲
	return TraversalTree(&BTree::ReadFileCallBack, true, true, &fstm);   // 遍历读取节点
}

bool BTree::IsSameString(const char *str1, const char *str2) {
	for (int i = 0, distance; str1[i] || str2[i]; i++) {           // 循环比较两个字符串直到结尾
		distance = str1[i] - str2[i];                              // 计算距离
		if (distance < 0) distance = -distance;                    // 距离取绝对值
		if (distance && distance != 'a' - 'A') return false;       // 两个字符不等，且也不是大小写关系
	}
	return true;
}

bool BTree::ReadFileCallBack(Node *node, std::fstream *fstm) {
	int i = TStack.top();                                  // 获得上一级路径
	if (i > 0) NewNode(fstm, node);                        // 如果不是第一个节点，用文件初始化值
	for (i = 0; i <= node->KeyNum; i++) {                  // 循环读取每个孩子指针
		if (node->Children[i]) {                           // 如果孩子指针不为空，说明写入时这个位置是有孩子的
			node->Children[i] = NewNode(i ? NULL : fstm);  // 那么分配一个空间给这个孩子节点并从文件流初始化第一个节点
			if (fstm->fail()) return false;                // 读取失败
			node->Children[i]->Parent = node;              // 孩子的父亲是当前节点
		} else break;                                      // 空指针代表节点没有更多孩子了，结束
	}
	return true;
}

bool BTree::SaveFileCallBack(Node *node, std::fstream *fstm) {
	fstm->write((char *)&node->KeyNum, sizeof(int));                    // 写键值数
	fstm->write((char *)node->Keys, sizeof(KeyType) * Degree);          // 写键值
	fstm->write((char *)node->Children, sizeof(Node *) * (Degree + 1)); // 写孩子指针
	return !fstm->fail();                                               // 返回失败位取反
}
// B 树文件操作部分函数结束

// B 树渲染函数部分开始
void BTree::DrawTree(Node *Target, int id, double time) {
	duration += std::chrono::steady_clock::now() - startTime;
	if (Tree) {  // 如果树不是空的
		InitialDraw = true;    // 第一个节点标志
		TraversalTree(&BTree::DrawTreeCallBack, false, true, Target, id);   // 遍历绘制树
		canvas->push(CANVAS_DONE, time >= 0, time >= 0 ? (float)time : 0.0f, 0.0f, 0.0f, 0.0f);  // 绘制完成，开始渲染
	} else canvas->push(CANVAS_CLEAN);  // 否则发送清屏指令
	startTime = std::chrono::steady_clock::now();
}

bool BTree::DrawTreeCallBack(Node *node, Node *Target, int id) {
	static Node *Parent;           // 记录当前父亲节点，用于判断是否是兄弟节点
	static float TreeHeight;       // 记录树高
	static float nowX, nowY;       // 记录当前坐标
	static float maxX;             // 记录最大横坐标
	static Stack<float> minX;      // 记录上次的最小横坐标
	static Stack<float> posX;      // 记录节点的中点坐标
	static int i, KeyNum;          // 记录下标和键值数
	static bool marked, needMark;  // 记录是否已经标红过键值、键值是否需要标红
	if (InitialDraw) {
		InitialDraw = false;
		TreeHeight = (TStack.size() + 1) * 1.5f - 0.5f;                   // 显示图形时的高度
		canvas->push(CANVAS_INIT, 1, 0.0f, 0.0f, TreeWidth, TreeHeight);  // 初始化画布大小
		maxX = nowY = nowX = 0.0f;  // 初始坐标为零
		Parent = node->Parent;      // 保存父节点
		minX.empty();      // 清空位置栈
		posX.empty();      // 清空节点中点栈
		minX.push(0.0f);   // 初始最小坐标为零
		marked = false;    // 未标红过键值
	}
	KeyNum = node->KeyNum ? node->KeyNum : 1;  // 储存键值数(空节点也绘制)
	if (node->Parent != Parent) {  // 如果父节点改变
		Parent = node->Parent;     // 设为新的父节点
		if (node->Children[0]) {   // 如果不是叶子节点
			nowY += 1.5f;          // 纵坐标向上走 1.5 格
			if (maxX < nowX) maxX = nowX;  // 如果最大横坐标比上次小横坐标，把值赋给它
			nowX = (minX.pop() + nowX - NODE_SPACE - KeyNum) / 2.0f;  // 计算当前横坐标
			if (!TStack.top()) minX.push(nowX);  // 如果当前节点是第一个节点，保存起始坐标为最小坐标
			if (node->KeyNum) {
				for (i = node->KeyNum; i >= 0; i--)
					canvas->push(DRAW_LINE, 0, nowX + i, nowY, posX.pop(), nowY - 0.5f);    // 画指针连线(非空节点)
			} else canvas->push(DRAW_LINE, 0, nowX + 0.5f, nowY, posX.pop(), nowY - 0.5f);  // 画指针连线(空节点)
		} else {              // 如果是叶子节点
			nowY = 0.0f;      // 纵坐标置零
			nowX = maxX;      // 当前横坐标变为最大横坐标
			minX.push(nowX);  // 保存当前横坐标为最小横坐标
		}
	}
	canvas->push(DRAW_NODE, KeyNum + 1,      // 键值个数
		nowX, nowY,                          // 绘制当前节点
		node == Target ? 0.0f : 1.0f, 0.0f   // 判断当前节点是否需要标红
	);
	for (i = 0; i < node->KeyNum; i++) {     // 循环绘制节点所有键值
		if (!marked) {
			needMark = Target == node && i == id;  // 如果还没有标红过键值，判断是否要标红当前键值
			if (needMark) marked = true;           // 设为已经标记过了
		} else if (needMark) needMark = false;     // 如果标记过了，则以后的键值都不标红
		canvas->push(DRAW_FONT, node->Keys[i],     // 键值
			nowX + i, nowY,
			needMark ? 0.0f : 1.0f, 0.0f           // 判断当前键值是否需要标红
		);
	}
	posX.push(KeyNum / 2.0f + nowX); // 记录当前节点的中点横坐标
	nowX += KeyNum + NODE_SPACE;     // 下一个节点的横坐标
	return true;
}
// B 树渲染函数部分结束