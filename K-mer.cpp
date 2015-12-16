#define KMER 7
#define STATE 4*4*4*4*4*4*4
#define COUNT 40
#define MAXN 1000000

#include <cstdio>
#include <iostream>     
#include <fstream>
#include <windows.h>
#include <string>      // 字符串类
#include <algorithm>   // STL 通用算法
#include <vector>      // STL 动态数组容器
#include <cctype>      // 字符处理
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
// 自定义getline函数
// buf：从当前地址开始搜索。
// len: 存放该行的字符串长度，不包含回车换行符。
// 返回值：下一次搜索开始的地址.如果到达文件尾，则返回NULL
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
// 创建镜像文件来读取文件，提高速度
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
			printf("打开文件失败");
		}
	}
	if (hFileMap == NULL)
	{
		hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
		if (hFileMap == NULL)
		{
			OutputDebugString(L"文件镜像句柄创建失败！");
			CloseHandle(hFile);
		}
	}
	if (lpbMapAddress == NULL)
	{
		lpbMapAddress = (char *)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 0);
		if (lpbMapAddress == NULL)
		{
			OutputDebugString(L"内存映射文件失败！");
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
	//释放
	UnmapViewOfFile(hFile);
	CloseHandle(hFileMap);

}

////////////////////////////////////////////////////////////////////////////
// 每一个线程如此创建，用于拆分读取文件每一行，根据字符串内容建立索引
// lpParameter：线程编号，区分读取空间
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
				//cout << pattern << "SUCCESS！LOCATION：" << hash_test.testid << endl;
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
	// 将文件与内存建立映像
	useMapFileReadText();

	//system("PAUSE");

	//////////////////////////////////////////////////////////////////////////     
	// 从内存中直取文件内容，建立索引，线程数COUNT
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
	// 查找测试     
	while (true) {
		char buff[6];
		cin >> buff;
		long pos = hash_test.GetHashTablePos(buff);
		myList[pos].outputList();
		pos != -1 ? cout << "查找测试字符串：" << buff << " 的散列位置：" << pos << endl : cout << buff << "存在冲突！" << endl;
		//myList[pos].outputList();
	}

	//system("pause");
	return 0;
}