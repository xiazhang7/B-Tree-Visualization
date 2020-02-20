#pragma once

#include <fstream>
#include <Windows.h>
#include <Commdlg.h>

#define FILE_FAIL 0             // 文件操作失败
#define FILE_TEXT_SUCCESS 1     // 文本文件操作成功
#define FILE_BINARY_SUCCESS 2   // 二进制文件操作成功
#define FILE_CANCEL 3           // 文件操作取消

namespace BTV {
	class File {
	public:

		typedef int  state;     // 定义文件返回状态类型

		File(void);             // 初始化

		state openFile(std::fstream &, const wchar_t *);  // 打开文件
		state saveFile(std::fstream &);                   // 保存文件

	private:

		typedef bool Type;              // 文件类型，0 为文本， 1 为二进制

		OPENFILENAME ofn;               // Windows 打开文件参数
		wchar_t fileName[MAX_PATH];     // 储存文件名
		Type getType(const wchar_t *);  // 获得文件类型

	};
}