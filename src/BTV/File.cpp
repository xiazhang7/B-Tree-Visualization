#include "File.h"

using namespace BTV;

File::File(void) {
	memset(&ofn, 0, sizeof(ofn));    // 填零初始化

	ofn.lStructSize  = sizeof(ofn);  // OPENFILENAME 结构体大小
	ofn.nFilterIndex = 1;            // 过滤索引
	ofn.lpstrFile    = fileName;     // 文件路径储存位置
	ofn.nMaxFile     = MAX_PATH;     // 最大路径长度
}

File::state File::openFile(std::fstream &infile, const wchar_t *path) {
	if (!path) {
		ofn.lpstrDefExt = NULL;                                                                        // 不指定文件默认扩展名
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;                          // 文件、目录必须存在，隐藏只读选项
		ofn.lpstrFilter = TEXT("所有文件\0*.*\0文本文件(*.txt)\0*.txt\0二进制文件(*.bin)\0*.bin\0");   // 打开文件筛选器
		if (!GetOpenFileName(&ofn)) return FILE_CANCEL;                                                // 用户取消打开文件
	}
	if (!path) path = fileName;    // 选择对应路径字符串操作
	Type type = getType(path);     // 获取文件类型
	infile.open(path, type ? (std::ios::in | std::ios::binary) : (std::ios::in));            // 判断打开方式并打开
	return infile.is_open() ? (type ? FILE_BINARY_SUCCESS : FILE_TEXT_SUCCESS) : FILE_FAIL;  // 返回打开方式、成功与否
}

File::state File::saveFile(std::fstream &outfile) {
	ofn.lpstrDefExt = TEXT("bin");                                            // 指定文件默认扩展名为 .bin
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;   // 目录必须存在，覆盖文件前发出警告
	ofn.lpstrFilter = TEXT("二进制文件(*.bin)\0*.bin\0");                     // 保存文件筛选器
	if (!GetSaveFileName(&ofn)) return FILE_CANCEL;                           // 用户取消保存文件
	outfile.open(fileName, std::ios::out | std::ios::binary);                 // 打开要写入二进制文件
	return outfile.is_open() ? FILE_BINARY_SUCCESS : FILE_FAIL;               // 返回打开成功与否
}

File::Type File::getType(const wchar_t *path) {
	int i = 0;
	while (path[i]) i++;
	if ((path[i - 3] == 't' || path[i - 3] == 'T')       // 判断后缀名，如果是 txt 就
		&& (path[i - 2] == 'x' || path[i - 3] == 'X')    // 返回 false, 表示是文本文件，
		&& (path[i - 1] == 't' || path[i - 3] == 'T')    // 否则返回 true, 是二进制文件
		&& path[i - 4] == '.')
		return false;
	return true;
}