#define KMER 7
#define STATE 4*4*4*4*4*4*4
#define COUNT 40
#define MAXN 1000000

#include <cstdio>
#include <iostream>     
#include <fstream>
#include <windows.h>
#include <string>      // �ַ�����
#include <algorithm>   // STL ͨ���㷨
#include <vector>      // STL ��̬��������
#include <cctype>      // �ַ�����
#include "HashAlgo.h"  
#include "strList.h"
#pragma warning(disable:4996)

using namespace std;

CHashAlgo hash_test(STATE);
strList myList[STATE];
HANDLE hMutex[STATE];
HANDLE console;
HANDLE Thread0[COUNT];
char fileContent[MAXN][101];
int s = 0;

////////////////////////////////////////////////////////////////////////////
// �Զ���getline����
// buf���ӵ�ǰ��ַ��ʼ������
// len: ��Ÿ��е��ַ������ȣ��������س����з���
// ����ֵ����һ��������ʼ�ĵ�ַ.��������ļ�β���򷵻�NULL
const char* _get_line(const char* buf, int* len)
{
	const char* tmp = buf;
	while (*tmp && (*tmp != 0x0d && *tmp != 0x0a && *tmp != '\n')) ++tmp;
	//while(*tmp && (*tmp != 0x0d || *tmp != 0x0a )) ++tmp;
	*len = tmp - buf; //

	if (*tmp == 0) return NULL;

	// skip New-Line char
	if (*tmp == 0x0d) { // Windows style New-Line 0x0d 0x0a
		tmp += 2;
		//assert(*tmp == 0x0a);
	}//else Unix style New-Line 0x0a
	else {
		++tmp;
	}

	return tmp;
}

////////////////////////////////////////////////////////////////////////////
// ���������ļ�����ȡ�ļ�������ٶ�
void useMapFileReadText() {

	HANDLE hFile = NULL, hFileMap = NULL;
	char * lpbMapAddress = NULL;
	int nFileSize = 0, nLeftSize = 0;
	if (hFile == NULL)
	{
		hFile = CreateFile(L"C:/Users/Bruno/Desktop/gene.txt", GENERIC_READ, 0, NULL,
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			printf("���ļ�ʧ��");
		}
	}
	if (hFileMap == NULL)
	{
		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hFileMap == NULL)
		{
			OutputDebugString(L"�ļ�����������ʧ�ܣ�");
			CloseHandle(hFile);
		}
	}
	if (lpbMapAddress == NULL)
	{
		lpbMapAddress = (char *)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (lpbMapAddress == NULL)
		{
			OutputDebugString(L"�ڴ�ӳ���ļ�ʧ�ܣ�");
			CloseHandle(hFileMap);
			CloseHandle(hFile);
		}
	}

	const char* buf = lpbMapAddress;
	const char* start = lpbMapAddress;
	int len;
	long i = 0;
	while (start != NULL) {
		start = _get_line(buf, &len);
		memcpy(fileContent[i], buf, 100);
		buf = start;
		i++;
	}
	cout << "Complete record 100,0000 DNA chains!" << endl;
	//�ͷ�
	UnmapViewOfFile(hFile);
	CloseHandle(hFileMap);

}

////////////////////////////////////////////////////////////////////////////
// ÿһ���߳���˴��������ڲ�ֶ�ȡ�ļ�ÿһ�У������ַ������ݽ�������
// lpParameter���̱߳�ţ����ֶ�ȡ�ռ�
DWORD WINAPI Fun2Proc(LPVOID lpParameter)
{

	int count = (int)lpParameter;

	long pos;

	char pattern[KMER + 1];
	
	for (int i = count * MAXN / COUNT; i < ( count + 1 ) * MAXN / COUNT; i++)
	{

		//cout << buffer << endl;
		for (int j = 0; j < 101 - KMER; j++) {
			for (int k = j; k < j + KMER; k++) {
				pattern[k - j] = fileContent[i][k];
			}
			pattern[KMER] = '\0';
			pos = hash_test.GetHashTablePos(pattern);
			if (pos == -1) {
			    hash_test.SetHashTable(pattern);
				long npos = hash_test.GetHashTablePos(pattern);
				myList[npos].insertList(i, j);
				//cout << pattern << "SUCCESS��LOCATION��" << hash_test.testid << endl;
			} else {
				WaitForSingleObject(hMutex[pos], INFINITE);
				myList[pos].insertList(i, j);
				ReleaseMutex(hMutex[pos]);
				//cout << "add " << pattern << " into location: " << pos << endl;
			}
		}

		WaitForSingleObject(console, INFINITE);
		if (s % 10000 == 0) {
			cout << s << "/1000000 has completed!" << endl;
		}
		s++;
		ReleaseMutex(console);

	}
}
   
int main(int argc, char**argv) {

	//////////////////////////////////////////////////////////////////////////     
	// ���ļ����ڴ潨��ӳ��
	useMapFileReadText();

	//system("PAUSE");

	//////////////////////////////////////////////////////////////////////////     
	// ���ڴ���ֱȡ�ļ����ݣ������������߳���COUNT
	for (int i = 0; i < STATE; i++) {
		hMutex[i] = CreateMutex(NULL, FALSE, NULL);
	}
	console = CreateMutex(NULL, FALSE, NULL);


	for (int i = 0; i < COUNT; i++) {
		Thread0[i] = CreateThread(NULL, 0, Fun2Proc, (void*)i, 0, NULL);
	}

	for (int i = 0; i < COUNT; i++) {
		CloseHandle(Thread0[i]);
	}

	//////////////////////////////////////////////////////////////////////////     
	// ���Ҳ���     
	while (true) {
		char buff[6];
		cin >> buff;
		long pos = hash_test.GetHashTablePos(buff);
		myList[pos].outputList();
		pos != -1 ? cout << "���Ҳ����ַ�����" << buff << " ��ɢ��λ�ã�" << pos << endl : cout << buff << "���ڳ�ͻ��" << endl;
		//myList[pos].outputList();
	}

	//system("pause");
	return 0;
}