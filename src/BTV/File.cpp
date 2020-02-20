#include "File.h"

using namespace BTV;

File::File(void) {
	memset(&ofn, 0, sizeof(ofn));    // �����ʼ��

	ofn.lStructSize  = sizeof(ofn);  // OPENFILENAME �ṹ���С
	ofn.nFilterIndex = 1;            // ��������
	ofn.lpstrFile    = fileName;     // �ļ�·������λ��
	ofn.nMaxFile     = MAX_PATH;     // ���·������
}

File::state File::openFile(std::fstream &infile, const wchar_t *path) {
	if (!path) {
		ofn.lpstrDefExt = NULL;                                                                        // ��ָ���ļ�Ĭ����չ��
		ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY;                          // �ļ���Ŀ¼������ڣ�����ֻ��ѡ��
		ofn.lpstrFilter = TEXT("�����ļ�\0*.*\0�ı��ļ�(*.txt)\0*.txt\0�������ļ�(*.bin)\0*.bin\0");   // ���ļ�ɸѡ��
		if (!GetOpenFileName(&ofn)) return FILE_CANCEL;                                                // �û�ȡ�����ļ�
	}
	if (!path) path = fileName;    // ѡ���Ӧ·���ַ�������
	Type type = getType(path);     // ��ȡ�ļ�����
	infile.open(path, type ? (std::ios::in | std::ios::binary) : (std::ios::in));            // �жϴ򿪷�ʽ����
	return infile.is_open() ? (type ? FILE_BINARY_SUCCESS : FILE_TEXT_SUCCESS) : FILE_FAIL;  // ���ش򿪷�ʽ���ɹ����
}

File::state File::saveFile(std::fstream &outfile) {
	ofn.lpstrDefExt = TEXT("bin");                                            // ָ���ļ�Ĭ����չ��Ϊ .bin
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;   // Ŀ¼������ڣ������ļ�ǰ��������
	ofn.lpstrFilter = TEXT("�������ļ�(*.bin)\0*.bin\0");                     // �����ļ�ɸѡ��
	if (!GetSaveFileName(&ofn)) return FILE_CANCEL;                           // �û�ȡ�������ļ�
	outfile.open(fileName, std::ios::out | std::ios::binary);                 // ��Ҫд��������ļ�
	return outfile.is_open() ? FILE_BINARY_SUCCESS : FILE_FAIL;               // ���ش򿪳ɹ����
}

File::Type File::getType(const wchar_t *path) {
	int i = 0;
	while (path[i]) i++;
	if ((path[i - 3] == 't' || path[i - 3] == 'T')       // �жϺ�׺��������� txt ��
		&& (path[i - 2] == 'x' || path[i - 3] == 'X')    // ���� false, ��ʾ���ı��ļ���
		&& (path[i - 1] == 't' || path[i - 3] == 'T')    // ���򷵻� true, �Ƕ������ļ�
		&& path[i - 4] == '.')
		return false;
	return true;
}