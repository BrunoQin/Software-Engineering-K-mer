#define KMER 10
#define STATE 4*4*4*4*4*4*4*4*4*4
#define COUNT 40
#define MAXN 1000000

#include <cstdio>
#include <iostream>     
#include <fstream>
#include <windows.h>
#include <string>      // 字符串类
#include <vector>      // STL 动态数组容器
#include <cctype>      // 字符处理
#include "HashAlgo.h"  
#include "strList.h"
#pragma warning(disable:4996)

using namespace std;

//CHashAlgo hash_test(STATE);
//strList myList[STATE];
//HANDLE hMutex[STATE];
HANDLE console;
HANDLE Thread0[COUNT];
//char fileContent[MAXN][101];
int fileNumber[MAXN][100];
float myHash[MAXN][101 - KMER];
int s = 0;
vector<strNode> final;
int hsum[10];
int hmin;
int hmax;
int group;

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
		//memcpy(fileContent[i], buf, 100);
		for (int j = 0; j < 100; j++) {
			switch (buf[j])
			{
			case 'A':
				fileNumber[i][j] = 0;
				break;
			case 'G':
				fileNumber[i][j] = 1;
				break;
			case 'C':
				fileNumber[i][j] = 2;
				break;
			case 'T':
				fileNumber[i][j] = 3;
				break;
			default:
				break;
			}
		}
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

	int a = 0;
	
	for (int i = count * MAXN / COUNT; i < ( count + 1 ) * MAXN / COUNT; i++)
	{

		//cout << buffer << endl;
		//for (int j = 0; j < 101 - KMER; j++) {
		//	for (int k = j; k < j + KMER; k++) {
		//		pattern[k - j] = fileContent[i][k];
		//	}
		//	pattern[KMER] = '\0';
		//	pos = hash_test.GetHashTablePos(pattern);
		//	if (pos == -1) {
		//	    hash_test.SetHashTable(pattern);
		//		long npos = hash_test.GetHashTablePos(pattern);
		//		//myList[npos].insertList(i, j);
		//		//cout << pattern << "SUCCESS！LOCATION：" << hash_test.testid << endl;
		//	} else {
		//		WaitForSingleObject(hMutex[pos], INFINITE);
		//		//myList[pos].insertList(i, j);
		//		ReleaseMutex(hMutex[pos]);
		//		//cout << "add " << pattern << " into location: " << pos << endl;
		//	}
		//}

		for (int j = 0; j < 101 - KMER; j++) {
			if (j == 0) {
				a = 0;
				for (int k = j; k < j + KMER; k++) {
					a = fileNumber[i][k] * pow(4, (KMER - (k - j) - 1)) + a;
				}
				myHash[i][j] = a;
			}
			else {
				myHash[i][j] = (myHash[i][j - 1] - fileNumber[i][j - 1] * pow(4, (KMER - 1))) * 4 + fileNumber[i][j + KMER - 1];
			}
		}

		WaitForSingleObject(console, INFINITE);
		if (s % 111111 == 0 && s != 0) {
			cout << (s / 111111) * 10 << "% has completed!" << endl;
		}
		s++;
		ReleaseMutex(console);

	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////
// 将字符串转换成4进制，在转换成10进制
// string:输入的字符串
// 返回值:得到的整数
int string2int(string tem) {
	int n = 0;
	int sum = 0;
	for (int i = 0; i < tem.size(); i++) {
		switch (tem[i])
		{
		case 'A':
			n = 0;
			break;
		case 'G':
			n = 1;
			break;
		case 'C':
			n = 2;
			break;
		case 'T':
			n = 3;
			break;
		default:
			break;
		}
		sum = sum + n * pow(4, tem.size() - i - 1);
	}
	return sum;
}

int r = 0;

////////////////////////////////////////////////////////////////////////////
// 每一个匹配线程如此创建，用于拆分读取输入字符串
// lpParameter：线程编号，区分读取空间
DWORD WINAPI Fun3Proc(LPVOID lpParameter)
{

	int count = (int)lpParameter; 

	vector<strNode> tem;

	for (int i = count * MAXN / COUNT; i < (count + 1) * MAXN / COUNT; i++)
	{

		for (int j = 1; j < 101 - KMER; j++) {
			if (myHash[i][j] == hsum[0]) {
				strNode *s = new strNode(i, j);
				tem.push_back(*s);
				delete s;
			}
		}

		WaitForSingleObject(console, INFINITE);
		r++;
		if (r % 111111 == 0 && r != 0) {
			cout << (r / 111111) * 10 << "% has completed!" << endl;
			if (r == 999999) {
				cout << "Please type anykey to check the results!" << endl;
			}
		}
		ReleaseMutex(console);

	}

	for (int j = 1; j < group; j++) {
		vector<strNode>::iterator itr = tem.begin();
		for (; itr != tem.end(); ) {
			if (itr->location + KMER * j > 90) {
				itr = tem.erase(itr);
			}
			else if (myHash[itr->fileNum][itr->location + KMER * j] != hsum[j]) {
				itr = tem.erase(itr);
			}
			else {
				++itr;
				continue;
			}
		}
	}

	vector<strNode>::iterator itr = tem.begin();
	for (; itr != tem.end(); ) {
		if (itr->location + KMER * group > 90) {
			itr = tem.erase(itr);
		}
		else if (myHash[itr->fileNum][itr->location + KMER * group] > hmax || myHash[itr->fileNum][itr->location + KMER * group] < hmin) {
			itr = tem.erase(itr);
		}
		else {
			++itr;
			continue;
		}
	}

	WaitForSingleObject(console, INFINITE);
	final.insert(final.end(), tem.begin(), tem.end());
	ReleaseMutex(console);
	return 0;
}
   
int main(int argc, char**argv) {

	//////////////////////////////////////////////////////////////////////////     
	// 将文件与内存建立映像
	useMapFileReadText();

	//////////////////////////////////////////////////////////////////////////     
	// 将字符串以10分组，并进行匹配
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

		string a;
		cin >> a;
		if (a.size() < 10) {
			int sum = string2int(a);
			int min = sum;
			int max = sum;
			for (int k = a.size(); k < 10; k++) {
				min = min * 4 + 0;
				max = max * 4 + 3;
			}
			cout << sum << "  " << min << "  " << max << "  " << endl;
			int d = 0;
			for (int i = 0; i < MAXN; i++) {
				for (int j = 0; j < 101 - KMER; j++) {
					if (myHash[i][j] >= min && myHash[i][j] <= max) {
						d++;
					}
				}
			}
			cout << d;
		}
		else if (a.size() == 10) {
			int sum = string2int(a);
			int d = 0;
			for (int i = 0; i < MAXN; i++) {
				for (int j = 0; j < 101 - KMER; j++) {
					if (myHash[i][j] == sum ) {
						d++;
					}
				}
			}
			cout << d;
		}
		else {
			group = a.size() / 10;
			string strGroup[10];

			for (int i = 0; i < group; i++) {
				strGroup[i] = a.substr(i * 10, 10);
			}
			strGroup[group] = a.substr(group * 10, a.size() - group * 10);

			for (int i = 0; i < group; i++) {
				hsum[i] = string2int(strGroup[i]);
			}

			int lastsum = string2int(strGroup[group]);
			hmin = lastsum;
			hmax = lastsum;
			for (int k = strGroup[group].size(); k < 10; k++) {
				hmin = hmin * 4 + 0;
				hmax = hmax * 4 + 3;
			}

			final.clear();

			for (int i = 0; i < COUNT; i++) {
				Thread0[i] = CreateThread(NULL, 0, Fun3Proc, (void*)i, 0, NULL);
			}

			for (int i = 0; i < COUNT; i++) {
				CloseHandle(Thread0[i]);
			}

			string lll;
			cin >> lll;

			cout << final.size();

			continue;
		}

		//myList[pos].outputList();

	}

	//system("pause");
	return 0;
}