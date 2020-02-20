#include "BTree.h"

using namespace BTV;

// B �������������ֺ�����ʼ
BTree::BTree(int Degree) : Degree(0), Tree(NULL), TreeWidth(0.0f), bar(NULL) { // ��ʼ����Ϊ 0, ��Ϊ�գ�ͼ�ο��Ϊ 0, ״̬��ָ��Ϊ��
	SetDegree(Degree);  // ���ý���
}

BTree::~BTree(void) {
	Destory();          // ɾ����ʱ���� B ��
}

bool BTree::SetDegree(int Degree) {
	if (Degree < 3) return false;     // ������СΪ 3
	if (Degree != this->Degree) {     // ���������һ��
		if (this->Degree) Destory();  // ���Ҳ��ǳ�ֵ���ݻ���
	} else return true;               // �����ò�����
	this->Degree = Degree;            // ���ý���
	MaxDegree = Degree - 1;           // ��������ֵ��
	MinDegree = MaxDegree / 2;        // ������С��ֵ��
	ShowDegree();                     // ״̬����ʾ��ǰ����
	return true;
}

BTree::Node *BTree::NewNode(std::fstream *fstm, Node *node) {
	if (!node) {
		node = new Node;                                  // �����½ڵ�
		node->Keys = new KeyType[Degree];                 // �����ֵ����ռ�
		node->Children = new Node*[Degree + 1]();         // ���亢��ָ�봢��ռ䲢����
	}
	if (fstm) {                                           // ������ļ��������ļ�����ʼ��
		fstm->read((char *)&node->KeyNum, sizeof(int));                    // ��ֵ��
		fstm->read((char *)node->Keys, sizeof(KeyType) * Degree);          // ��ֵ
		fstm->read((char *)node->Children, sizeof(Node *) * (Degree + 1)); // ����ָ��
	}
	return node;
}

bool BTree::DelNode(Node *node) {
	if (!node) return false;    // ��ָ�룬����ʧ��
	delete [](node->Keys);      // ɾ����ֵ����ռ�
	delete [](node->Children);  // ɾ������ָ�봢��ռ�
	delete node;                // ɾ���ڵ�
	return true;
}

void BTree::Print(bool parentFirst) {
	canvas->push(STATUS_PRINT, parentFirst);                       // ��ʾ��ǰ����
	duration = std::chrono::microseconds::zero();                  // ����ʱ������
	startTime = std::chrono::steady_clock::now();                  // ��¼��ʼʱ��
	TraversalTree(&BTree::PrintTreeCallBack, parentFirst, false);  // ������ӡ B ��
	duration += std::chrono::steady_clock::now() - startTime;      // ����ʱ���
	DrawTree(NULL, -1, duration.count());                          // ��ӡ����״̬
}

bool BTree::PrintTreeCallBack(Node *node) {
	for(int i = 0; i < node->KeyNum; i++) DrawTree(node, i);       // ��Ⱦ��ǰ�ڵ����м�ֵ
	return true;
}

void BTree::Destory(void) {
	TraversalTree(&BTree::DelNode, false);  // ����ɾ��ÿ���ڵ�
	Tree = NULL;                            // ����ָ���ÿ�
	TreeWidth = 0.0f;                       // ����ͼ�ο��
	DrawTree();                             // ��ʾ�������ջ�����
}

template<typename ...Ts>
bool BTree::TraversalTree(bool (BTree::*CallBack)(Node *, Ts ...args), bool parentFirst, bool useTStack, Ts ...args) {
	if (!Tree) return true;               // ���������ñ���
	Node *node = Tree, *Parent;           // ��������ʼ����
	Stack<int> *stack;
	if (useTStack) stack = &TStack;       // ѡ��ʹ�õ�·��ջ
	else stack = &IdStack;
	if (parentFirst)
		if (!(this->*CallBack)(node, std::forward<Ts>(args)...))
			return false;                 // �����ȶԸ��ڵ����������ʧ������ֹ
	int id = 0;                           // ������˽ڵ㿪ʼ����
	stack->empty();                       // ���·��ջ
	while (true) {
		if (node->Children[id]) {         // �к��ӣ��������±���
			node = node->Children[id];    // ���ӽڵ���
			stack->push(id);              // ��¼��ǰ·��
			if(parentFirst)
				if (!(this->*CallBack)(node, std::forward<Ts>(args)...))
					return false;         // �����ȶԸýڵ����������ʧ������ֹ
			id = 0;                       // �ӽڵ������˽ڵ㿪ʼ����
		} else {                          // û�к��ӣ�˵����Ҷ�ڵ㣬׼�����ظ��ڵ�
			do {
				Parent = node->Parent;    // �ȱ��游�ڵ㣬��ֹ�ڵ㱻ɾ�Ҳ�������
				if(!parentFirst)
					if(!(this->*CallBack)(node, std::forward<Ts>(args)...))
						return false;     // ����û�к���׼�����ظ��ڵ�ʱ�����ýڵ㣬����ʧ������ֹ
				id = stack->pop() + 1;    // ���ظ��ڵ�������һ���ӽڵ�
				if (!id) return true;     // ��� id + 1 ���� 0, ˵��ջ�գ���������
				node = Parent;            // ���ظ��ڵ�
			} while (id > node->KeyNum);  // ���Խ��ͼ��������ڵ���
		}
	}
	return true;
}

void BTree::ShowDegree(void) {
	if (bar) bar->msg(DEGREE_STATUSBAR, L"������%d", Degree);  // ��ʾ��ʼ
}

void BTree::SetStatusBar(StatusBar *bar) {
	this->bar = bar;  // ����״̬�����
	ShowDegree();     // ��ʾ����
}
// B �������������ֺ�������

// B �����벿�ֺ�����ʼ
bool BTree::Insert(KeyType Key) {
	if (Degree < 3) return false;      // ��δ���û�̫С
	canvas->push(STATUS_INSERT, Key);  // ��ʾ��ǰ����
	duration = std::chrono::microseconds::zero();  // ����ʱ������
	startTime = std::chrono::steady_clock::now();  //��¼��ʼʱ��
	if (!Tree) {                       // ������ǿյ�
		Tree = NewNode();              // �����½ڵ�
		Tree->KeyNum = 0;              // �ڵ�ֵ��Ϊ 0
		Tree->Parent = NULL;           // ���ڵ㣬û�и���
	}
	Node *node = Tree;
	int i = NodeSearch(node, Key);     // ����Ԫ�ظô�ŵ�λ��
	IdStack.empty();                   // ���·��ջ
	while (node->Children[0]) {        // ����Ҷ�ӽڵ��һֱ����
		IdStack.push(i);               // ��¼·��
		node = node->Children[i];      // ����һ���ڵ���
		i = NodeSearch(node, Key);     // ����Ԫ�ظô�ŵ�λ��
	}
	NodeInsert(node, Key, i);          // �ҵ��ڵ㣬����ֵ
	duration += std::chrono::steady_clock::now() - startTime;  // ����ʱ���
	DrawTree(NULL, -1, duration.count());                      // ��ӡ����״̬
	return true;
}

int BTree::NodeSearch(Node *node, KeyType Key) {
	int i;
	for (i = 0; i < node->KeyNum && Key > node->Keys[i]; i++)   // ����ָ��λ�ã���ֵ������Ⱦ����
		DrawTree(node, i);
	if (i < node->KeyNum && Key == node->Keys[i])  // ����ҵ�Ŀ���ֵ
		DrawTree(node, i);        // �ڵ㡢��ֵ���
	else if (!i && node->KeyNum)  // ��� i Ϊ�㲢�ҽڵ㲻�ǿսڵ�
		DrawTree(node);           // �ڵ���
	return i;
}

void BTree::NodeInsert(Node *node, KeyType Key, int i, Node *child, bool rightChild) {
	int end;
	for (end = node->KeyNum; end > i; end--)
		node->Keys[end] = node->Keys[end - 1];               // ÿ���ڵ����һλ
	node->Keys[i] = Key;                                     // ָ��λ�ò���ֵ
	node->KeyNum++;                                          // �ڵ㺬�м�ֵ�� +1
	if (child) {                                             // ����������к���ָ��
		if (rightChild) i++;                                 // ��������Һ��ӣ�ĩλ��һ
		for (end = node->KeyNum; end > i; end--)
			node->Children[end] = node->Children[end - 1];   // ÿ������ָ�����һλ
		node->Children[end] = child;                         // ���������ֵ���Һ���
	}
	if(!node->Children[0]) TreeWidth += 1.0f;                // �����Ҷ�ӽڵ��ϲ����ֵ��ͼ�ο�ȼ�һ
	if (node->KeyNum == Degree) {
		canvas->push(STATUS_FORK, Degree);   // ״̬����ʾ�ڵ����
		DrawTree(node);                      // ����֮ǰ��Ⱦһ������״̬
		Fork(node);                          // ��������ڵ����˾ͷ���
	}
}

void BTree::Fork(Node *node1) {
	int i, j;
	Node *node2 = NewNode();                                 // ����ռ�����ѵ��½ڵ�
	bool hasChild = node1->Children[0] ? true : false;       // �жϽڵ��Ƿ��к���
	for (i = MinDegree + 1, j = 0; i < Degree; i++, j++) {   // �ƶ�ԭ�ڵ��벿�ֵ��½ڵ�
		node2->Keys[j] = node1->Keys[i];                     // �ƶ���ֵ
		if (hasChild) {                                      // ����к���
			node2->Children[j] = node1->Children[i];         // �ƶ�����ָ��
			node1->Children[i] = NULL;                       // �ÿ�ԭλ�õ�ָ��
			node2->Children[j]->Parent = node2;              // �����ӽڵ�ĸ���Ϊ�½ڵ�
		}
	}
	if (hasChild) {
		node2->Children[j] = node1->Children[i];  // ʣ��һ������ָ�뵥���ƶ�
		node1->Children[i] = NULL;                // �����ÿ�
		node2->Children[j]->Parent = node2;       // �������¸�ָ��
	} else TreeWidth -= 1.0f - NODE_SPACE;        // Ҷ�ӽڵ���ѣ�ͼ�ο�ȼ���
	node1->KeyNum = MinDegree;                    // ����ԭ�ڵ��ֵ��
	node2->KeyNum = MaxDegree - MinDegree;        // �����½ڵ��ֵ��
	if (node1->Parent) {                          // ������Ǹ��ڵ�
		node2->Parent = node1->Parent;            // �½ڵ�ĸ��׾���ԭ�ڵ�ĸ���
		NodeInsert(                               // �м�Ԫ�����ϲ��븸�ڵ���
			node1->Parent,
			node1->Keys[MinDegree],
			IdStack.pop(),
			node2
		);
	} else {                                      // ����Ǹ��ڵ�
		Tree = NewNode();                         // �½�һ�����ڵ�
		Tree->Parent = NULL;                      // ���ڵ�û�и���
		node1->Parent = node2->Parent = Tree;     // ���ѳ����������ڵ�ĸ��׶��Ǹ��ڵ�
		Tree->Keys[0] = node1->Keys[MinDegree];   // ���ڵ�ĵ�һ��Ԫ�ص�ԭ�ڵ���м�ڵ�
		Tree->Children[0] = node1;                // ��һ��Ԫ�ص�������ԭ�ڵ�
		Tree->Children[1] = node2;                // ��һ��Ԫ�ص��Һ������½ڵ�
		Tree->KeyNum = 1;                         // ���ڵ�����ֻ��һ��Ԫ��
	}
}
// B �����벿�ֺ�������

// B ���������ֺ�����ʼ
BTree::Node *BTree::TreeSearch(KeyType Key, int *id) {
	int i;
	IdStack.empty();                   // ���·��ջ
	for (Node *node = Tree; node; node = node->Children[i]) {
		i = NodeSearch(node, Key);     // Ѱ�Ҹ�ֵ�ڽڵ���Ӧ�ô���λ��
		if (i < node->KeyNum && node->Keys[i] == Key) {    // ����ҵ���
			canvas->push(STATUS_RSEARCH, 1);  // ״̬����ʾ�ҵ�
			if(id) *id = i;            // ��¼ֵ�ڽڵ��е�λ��
			return node;               // ����ֵ���ڽڵ�
		}
		IdStack.push(i);               // ��¼·��
	}
	canvas->push(STATUS_RSEARCH, 0);   // ״̬����ʾδ�ҵ�
	return NULL;      // �Ҳ��������ؿ�ָ��
}

bool BTree::Search(KeyType Key) {
	canvas->push(STATUS_SEARCH, Key);                          // ��ʾ��ǰ����
	duration = std::chrono::microseconds::zero();              // ����ʱ������
	startTime = std::chrono::steady_clock::now();              // ��¼��ʼʱ��
	bool result = TreeSearch(Key);                             // �����ڵ�
	duration += std::chrono::steady_clock::now() - startTime;  // ����ʱ���
	DrawTree(NULL, -1, duration.count());                      // ��ӡ����״̬
	return result;
}
// B ���������ֺ�������

// B ��ɾ�����ֺ�����ʼ
bool BTree::Delete(KeyType Key) {
	int id;
	canvas->push(STATUS_DELETE, Key);                              // ��ʾ��ǰ����
	duration = std::chrono::microseconds::zero();                  // ����ʱ������
	startTime = std::chrono::steady_clock::now();                  // ��¼��ʼʱ��
	Node *node = TreeSearch(Key, &id);                             // �������ҵ���ǰֵ���ڽڵ�
	if (!node) {
		duration += std::chrono::steady_clock::now() - startTime;  // ����ʱ���
		DrawTree(NULL, -1, duration.count());                      // ɾ��ʧ�ܣ���Ⱦ����״̬
		return false;                                              // �Ҳ����ڵ���ɾ��ʧ��
	}
	if (node->Children[0]) {                                       // �������ڵ㲻��Ҷ�ӽڵ�
		IdStack.push(id);                                          // ���ʹ˽ڵ�λ�õ�·��ջ��
		Node *LeafNode = FindMaxLeaf(node->Children[id]);          // �ҵ��ȵ�ǰֵС�����ֵ����Ҷ�ӽڵ�
		node->Keys[id] = LeafNode->Keys[LeafNode->KeyNum - 1];     // ��������ֵ������ǰ��
		node = LeafNode, id = LeafNode->KeyNum - 1;                // �����ڵ���Ҷ�ӽڵ�
	}
	NodeDelete(node, id);   // ɾ����Ӧ��ֵ
	while (node->KeyNum < MinDegree && node->Parent) {  // ����ֵ��С����СҪ��������ڵ㲻�Ǹ��ڵ�ʱ
		id = IdStack.pop();                         // �ҵ��˽ڵ���һ���ڵ�ĺ���λ��
		if (BorrowKey(node, id)) {                  // ���ֵܽ赽ֵ,���´�
			break;                                  // �����жϣ���Ϊ���ڵ��ֵ������
		} else {                                    // �費��ֵ
			canvas->push(STATUS_MERGE, MinDegree);  // ״̬����ʾ�ϲ���Ϣ
			Node *Parent = node->Parent;            // ���游�ڵ㣬��ֹ��ǰ�ڵ㱻ɾ�Ҳ�������
			Merge(node, id);                        // �Ҹ��ֵܽڵ�ϲ�
			node = Parent;                          // �����ڵ��ߣ��������ڵ��ǲ�������Ҫ��
			DrawTree(node);                         // �ϲ���ɣ���Ⱦ����״̬
		}
	}
	if (node == Tree && !node->KeyNum) {   // �������ڵ��Ǹ��ڵ㲢�Ҽ�ֵ��Ϊ 0
		if (Tree->Children[0]) {
			Tree = Tree->Children[0];      // ������ڵ��к��ӣ����ӱ�ɸ��ڵ�
			Tree->Parent = NULL;           // ���ڵ�û�и���
		} else Tree = NULL;                // �����������Ϳ���
		DelNode(node);                     // ɾ������ڵ�
	}
	duration += std::chrono::steady_clock::now() - startTime;  // ����ʱ���
	DrawTree(NULL, -1, duration.count());                      // ɾ����ɣ���Ⱦ����״̬
	return true;
}

BTree::KeyType BTree::NodeDelete(Node *node, int i, Node **child, bool rightChild) {
	KeyType Key = node->Keys[i];                           // ����Ҫɾ����ֵ
	int id;
	for (id = i; id < node->KeyNum - 1; id++)
		node->Keys[id] = node->Keys[id + 1];               // ѭ����ǰ�ƶ���ֵ��ɾ����ֵ��
	if (node->Children[0]) {                               // ����к��ӣ�ɾ����Ӧ����ָ��
		id = rightChild ? i + 1 : i;                       // �ж�Ҫ�������ӻ����Һ���
		if (child) *child = node->Children[id];            // ����ɾ���ĺ��ӣ������Ҫ��
		for (; id < node->KeyNum; id++)
			node->Children[id] = node->Children[id + 1];   // ѭ����ǰ�ƶ�����ָ�루ɾ������ָ�룩
	} else TreeWidth -= 1.0f;                              // ɾ����ֵ��ͼ�ο�ȼ�һ
	node->Children[id] = NULL;   // ���һ��ָ���ÿ�
	node->KeyNum--;              // ��ֵ����һ
	return Key;                  // ���ر�ɾ�ļ�ֵ
}

BTree::Node *BTree::FindMaxLeaf(Node *node) {
	while (node->Children[0]) {               // ���ڵ��к���
		DrawTree(node);                       // ��Ⱦ��������ֵ�Ĺ���
		IdStack.push(node->KeyNum);           // ��¼·��
		node = node->Children[node->KeyNum];  // ������ӷ�����
	}
	DrawTree(node, node->KeyNum - 1);         // ��Ⱦ���ҵ�����ֵ��״̬
	return node;
}

bool BTree::BorrowKey(Node *node, int id) {
	Node *Parent = node->Parent, *Target, *Temp = NULL;
	KeyType Key;
	if (id && Parent->Children[id - 1]->KeyNum > MinDegree) {  // �����ֵܽ�ֵ
		canvas->push(STATUS_BORROW, 0);                                   // ״̬����ʾ��ֵ��Ϣ
		id--;                                                             // �������ֵ�
		Target = Parent->Children[id];                                    // Ŀ�������ֵ�
		Key = Parent->Keys[id];                                           // ���游��ֵ
		Parent->Keys[id] = NodeDelete(Target, Target->KeyNum - 1, &Temp); // ���ֵܵ����ֵ�����׼����������Һ���ָ��
		NodeInsert(node, Key, 0, Temp, false);                            // �Ѹ���ֵ�ͺ���ָ����뵱ǰ�ڵ㿪ͷ
	} else if (id != Parent->KeyNum && Parent->Children[id + 1]->KeyNum > MinDegree) {  // �����ֵܽ�ֵ,������ͬ
		canvas->push(STATUS_BORROW, 1);                                   // ״̬����ʾ��ֵ��Ϣ
		id++;                                                             // �������ֵ�
		Target = Parent->Children[id];                                    // Ŀ�������ֵ�
		Key = Parent->Keys[id - 1];                                       // ���游��ֵ
		Parent->Keys[id - 1] = NodeDelete(Target, 0, &Temp, false);       // ���ֵܵ����ֵ�����׼�������������ָ��
		NodeInsert(node, Key, node->KeyNum, Temp);                        // �Ѹ���ֵ�ͺ���ָ����뵱ǰ�ڵ��β
	} else return false;            // �޷��������ֵܽ�ֵ������ʧ��
	if(Temp) Temp->Parent = node;   // �޸ĺ���ָ��ĸ���Ϊ��ǰ�ڵ�
	return true;                    // �ɹ��赽ֵ
}

void BTree::Merge(Node *node, int id) {
	Node *Target;
	int j = 0;
	if (id) Target = node->Parent->Children[--id];                  // ��ǰ�ڵ㲻�ǵ�һ���ڵ㣬Ŀ��ڵ���Ϊ���ֵ�
	else Target = node, node = node->Parent->Children[id + 1];      // �ǵ�һ���ڵ㣬Ŀ��ڵ���Ϊ��ǰ�ڵ㣬��ǰ�ڵ���Ϊ���ֵ�
	Target->Keys[Target->KeyNum] = NodeDelete(Target->Parent, id);  // ����ֵ׷�ӵ�Ŀ��ڵ�
	bool hasChild = node->Children[0] ? true : false;               // �жϵ�ǰ�ڵ��Ƿ��к���
	for (id = Target->KeyNum + 1; j < node->KeyNum; id++, j++) {    // ѭ��������ǰ�ڵ�
		Target->Keys[id] = node->Keys[j];                           // ��ֵ׷�ӵ�Ŀ��ڵ�
		if (hasChild) {
			Target->Children[id] = node->Children[j];               // �к��ӣ�����ָ��׷�ӵ�Ŀ��ڵ�
			Target->Children[id]->Parent = Target;                  // �޸ĺ��ӽڵ�ĸ���ΪĿ��ڵ�
		}
	}
	if (hasChild) {
		Target->Children[id] = node->Children[j]; // ʣ��һ��ָ�뵥��׷��
		Target->Children[id]->Parent = Target;    // �����޸ĸ���ָ��
	} else TreeWidth += 1.0f - NODE_SPACE;        // �ϲ��ڵ㣬ͼ�ο������
	Target->KeyNum = id;  // ����Ŀ��ڵ�ļ�ֵ����
	DelNode(node);        // ɾ����ǰ�ڵ�
}
// B ��ɾ�����ֺ�������

// B ���ļ��������ֺ�����ʼ
bool BTree::Open(std::fstream &fstm, bool isBinary) {
	bool result;
	if (isBinary) {
		duration = std::chrono::microseconds::zero();              // ����ʱ������
		startTime = std::chrono::steady_clock::now();              // ��¼��ʼʱ��
		result = HandleBinaryFile(fstm);                           // �������ļ�����
		duration += std::chrono::steady_clock::now() - startTime;  // ����ʱ���
		DrawTree(NULL, -1, duration.count());                      // ������� B ��
	} else result = HandleTextFile(fstm);    // �ı��ļ�����
	fstm.close();                            // �ر��ļ���
	return result;                           // ���ؽ��
}

bool BTree::Save(std::fstream &fstm) {
	const long long checkHeader = CHECK_CODE;
	fstm.write((char *)&checkHeader, sizeof(long long));  // д��У��ͷ
	fstm.write((char *)&Degree, sizeof(int));             // д�����
	fstm.write((char *)&TreeWidth, sizeof(float));        // д��ͼ�ο��
	if (fstm.fail()) return false;                        // д��ʧ��
	bool result = TraversalTree(&BTree::SaveFileCallBack, true, true, &fstm);  // �ݹ�д�����нڵ�
	fstm.close();                // �ر��ļ���
	if (!result) return false;   // �ݹ�д��ʱ����
	return true;
}

bool BTree::HandleTextFile(std::fstream &fstm) {
	char headStr[7];                                                 // ����ǰ�벿���ַ���
	fstm >> headStr;                                                 // ��������ַ���
	if (!IsSameString(headStr, "degree")) return false;              // �ж��ַ����ǲ���"degree"����У������
	int tailNum;                                                     // �����벿������
	if (!(fstm >> tailNum) || !SetDegree(tailNum)) return false;     // �޷�������������������ʧ����ֹ
	while (fstm >> headStr >> tailNum) {                             // ѭ����ȡ������ֵ
		if (IsSameString(headStr, "insert")) Insert(tailNum);        // ����
		else if (IsSameString(headStr, "delete")) Delete(tailNum);   // ɾ��
		else if (IsSameString(headStr, "search")) Search(tailNum);   // ����
		else if (IsSameString(headStr, "print")) Print(tailNum);     // ��ӡ
		else return false;                                           // �Ҳ�����Ӧ���ܣ�ִ��ʧ��
	}
	return true;
}

bool BTree::HandleBinaryFile(std::fstream &fstm) {
	long long checkHeader;
	fstm.read((char *)&checkHeader, sizeof(long long));  // �����ļ�У��ͷ
	if (checkHeader != CHECK_CODE) return false;         // �ļ�У��ʧ��
	if (Tree) Destory();                                 // ��������ǿյģ��ȴݻ�
	int Degree;
	fstm.read((char *)&Degree, sizeof(int));       // ������
	if(!SetDegree(Degree)) return false;           // ���ý���
	fstm.read((char *)&TreeWidth, sizeof(float));  // ����ͼ�ο��
	Tree = NewNode(&fstm);                         // �½�һ�����ڵ㲢���ļ�����ʼ��
	Tree->Parent = NULL;                           // ���ڵ��޸���
	return TraversalTree(&BTree::ReadFileCallBack, true, true, &fstm);   // ������ȡ�ڵ�
}

bool BTree::IsSameString(const char *str1, const char *str2) {
	for (int i = 0, distance; str1[i] || str2[i]; i++) {           // ѭ���Ƚ������ַ���ֱ����β
		distance = str1[i] - str2[i];                              // �������
		if (distance < 0) distance = -distance;                    // ����ȡ����ֵ
		if (distance && distance != 'a' - 'A') return false;       // �����ַ����ȣ���Ҳ���Ǵ�Сд��ϵ
	}
	return true;
}

bool BTree::ReadFileCallBack(Node *node, std::fstream *fstm) {
	int i = TStack.top();                                  // �����һ��·��
	if (i > 0) NewNode(fstm, node);                        // ������ǵ�һ���ڵ㣬���ļ���ʼ��ֵ
	for (i = 0; i <= node->KeyNum; i++) {                  // ѭ����ȡÿ������ָ��
		if (node->Children[i]) {                           // �������ָ�벻Ϊ�գ�˵��д��ʱ���λ�����к��ӵ�
			node->Children[i] = NewNode(i ? NULL : fstm);  // ��ô����һ���ռ��������ӽڵ㲢���ļ�����ʼ����һ���ڵ�
			if (fstm->fail()) return false;                // ��ȡʧ��
			node->Children[i]->Parent = node;              // ���ӵĸ����ǵ�ǰ�ڵ�
		} else break;                                      // ��ָ�����ڵ�û�и��ຢ���ˣ�����
	}
	return true;
}

bool BTree::SaveFileCallBack(Node *node, std::fstream *fstm) {
	fstm->write((char *)&node->KeyNum, sizeof(int));                    // д��ֵ��
	fstm->write((char *)node->Keys, sizeof(KeyType) * Degree);          // д��ֵ
	fstm->write((char *)node->Children, sizeof(Node *) * (Degree + 1)); // д����ָ��
	return !fstm->fail();                                               // ����ʧ��λȡ��
}
// B ���ļ��������ֺ�������

// B ����Ⱦ�������ֿ�ʼ
void BTree::DrawTree(Node *Target, int id, double time) {
	duration += std::chrono::steady_clock::now() - startTime;
	if (Tree) {  // ��������ǿյ�
		InitialDraw = true;    // ��һ���ڵ��־
		TraversalTree(&BTree::DrawTreeCallBack, false, true, Target, id);   // ����������
		canvas->push(CANVAS_DONE, time >= 0, time >= 0 ? (float)time : 0.0f, 0.0f, 0.0f, 0.0f);  // ������ɣ���ʼ��Ⱦ
	} else canvas->push(CANVAS_CLEAN);  // ����������ָ��
	startTime = std::chrono::steady_clock::now();
}

bool BTree::DrawTreeCallBack(Node *node, Node *Target, int id) {
	static Node *Parent;           // ��¼��ǰ���׽ڵ㣬�����ж��Ƿ����ֵܽڵ�
	static float TreeHeight;       // ��¼����
	static float nowX, nowY;       // ��¼��ǰ����
	static float maxX;             // ��¼��������
	static Stack<float> minX;      // ��¼�ϴε���С������
	static Stack<float> posX;      // ��¼�ڵ���е�����
	static int i, KeyNum;          // ��¼�±�ͼ�ֵ��
	static bool marked, needMark;  // ��¼�Ƿ��Ѿ�������ֵ����ֵ�Ƿ���Ҫ���
	if (InitialDraw) {
		InitialDraw = false;
		TreeHeight = (TStack.size() + 1) * 1.5f - 0.5f;                   // ��ʾͼ��ʱ�ĸ߶�
		canvas->push(CANVAS_INIT, 1, 0.0f, 0.0f, TreeWidth, TreeHeight);  // ��ʼ��������С
		maxX = nowY = nowX = 0.0f;  // ��ʼ����Ϊ��
		Parent = node->Parent;      // ���游�ڵ�
		minX.empty();      // ���λ��ջ
		posX.empty();      // ��սڵ��е�ջ
		minX.push(0.0f);   // ��ʼ��С����Ϊ��
		marked = false;    // δ������ֵ
	}
	KeyNum = node->KeyNum ? node->KeyNum : 1;  // �����ֵ��(�սڵ�Ҳ����)
	if (node->Parent != Parent) {  // ������ڵ�ı�
		Parent = node->Parent;     // ��Ϊ�µĸ��ڵ�
		if (node->Children[0]) {   // �������Ҷ�ӽڵ�
			nowY += 1.5f;          // ������������ 1.5 ��
			if (maxX < nowX) maxX = nowX;  // �������������ϴ�С�����꣬��ֵ������
			nowX = (minX.pop() + nowX - NODE_SPACE - KeyNum) / 2.0f;  // ���㵱ǰ������
			if (!TStack.top()) minX.push(nowX);  // �����ǰ�ڵ��ǵ�һ���ڵ㣬������ʼ����Ϊ��С����
			if (node->KeyNum) {
				for (i = node->KeyNum; i >= 0; i--)
					canvas->push(DRAW_LINE, 0, nowX + i, nowY, posX.pop(), nowY - 0.5f);    // ��ָ������(�ǿսڵ�)
			} else canvas->push(DRAW_LINE, 0, nowX + 0.5f, nowY, posX.pop(), nowY - 0.5f);  // ��ָ������(�սڵ�)
		} else {              // �����Ҷ�ӽڵ�
			nowY = 0.0f;      // ����������
			nowX = maxX;      // ��ǰ�������Ϊ��������
			minX.push(nowX);  // ���浱ǰ������Ϊ��С������
		}
	}
	canvas->push(DRAW_NODE, KeyNum + 1,      // ��ֵ����
		nowX, nowY,                          // ���Ƶ�ǰ�ڵ�
		node == Target ? 0.0f : 1.0f, 0.0f   // �жϵ�ǰ�ڵ��Ƿ���Ҫ���
	);
	for (i = 0; i < node->KeyNum; i++) {     // ѭ�����ƽڵ����м�ֵ
		if (!marked) {
			needMark = Target == node && i == id;  // �����û�б�����ֵ���ж��Ƿ�Ҫ��쵱ǰ��ֵ
			if (needMark) marked = true;           // ��Ϊ�Ѿ���ǹ���
		} else if (needMark) needMark = false;     // �����ǹ��ˣ����Ժ�ļ�ֵ�������
		canvas->push(DRAW_FONT, node->Keys[i],     // ��ֵ
			nowX + i, nowY,
			needMark ? 0.0f : 1.0f, 0.0f           // �жϵ�ǰ��ֵ�Ƿ���Ҫ���
		);
	}
	posX.push(KeyNum / 2.0f + nowX); // ��¼��ǰ�ڵ���е������
	nowX += KeyNum + NODE_SPACE;     // ��һ���ڵ�ĺ�����
	return true;
}
// B ����Ⱦ�������ֽ���