#pragma once

#include <fstream>
#include <Windows.h>
#include <Commdlg.h>

#define FILE_FAIL 0             // �ļ�����ʧ��
#define FILE_TEXT_SUCCESS 1     // �ı��ļ������ɹ�
#define FILE_BINARY_SUCCESS 2   // �������ļ������ɹ�
#define FILE_CANCEL 3           // �ļ�����ȡ��

namespace BTV {
	class File {
	public:

		typedef int  state;     // �����ļ�����״̬����

		File(void);             // ��ʼ��

		state openFile(std::fstream &, const wchar_t *);  // ���ļ�
		state saveFile(std::fstream &);                   // �����ļ�

	private:

		typedef bool Type;              // �ļ����ͣ�0 Ϊ�ı��� 1 Ϊ������

		OPENFILENAME ofn;               // Windows ���ļ�����
		wchar_t fileName[MAX_PATH];     // �����ļ���
		Type getType(const wchar_t *);  // ����ļ�����

	};
}