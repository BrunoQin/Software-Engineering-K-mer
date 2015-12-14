#define KMER 5
#define STATE 4*4*4*4*4

#include <iostream>     
#include <fstream>
#include <memory.h>
#include <math.h>
#include "HashAlgo.h"  
#include "strList.h"
#include <ctime>
#pragma warning(disable:4996)
using namespace std;

const int MAXN = 10000000;
const int MAXS = 60 * 1024 * 1024;

int numbers[MAXN];
char buf[MAXS];


void fread_analyse()
{
	int start = clock();

	/*freopen("C:/Users/Bruno/Desktop/gene.txt", "rb", stdin);
	int len = fread(buf, 1, MAXS, stdin);
	buf[len] = '\0';*/
	ifstream in("/Users/macbookpro/Downloads/gene.txt", ios::in);
	char buffer[1024];
	for (int i = 0; i < 10000; i++) {
		in.getline(buffer, 1024);
	}

	printf("%.3lf\n", double(clock() - start) / CLOCKS_PER_SEC);

}
   
int main(int argc, char**argv) {

	fread_analyse();

	CHashAlgo hash_test(STATE);

	cout << "ȡ�ó�ʼ��ɢ��������Ϊ��" << hash_test.GetTableLength() << endl;

	bool is_success;
	long pos;

	strList myList[STATE];

	ifstream in("C:/Users/Bruno/Desktop/gene.txt", ios::in);
	char buffer[200];
	

	//////////////////////////////////////////////////////////////////////////     
	// ɢ�н���     
	for (int i = 0; i < 10000; i++) {
		in.getline(buffer, 200);
		//cout << buffer << endl;
		for (int j = 0; j < 101 - KMER; j++) {
			char pattern[KMER + 1];
			for (int k = j; k < j + KMER; k++) {
				pattern[k - j] = buffer[k];
			}
			pattern[KMER] = '\0';
			long  pos = hash_test.GetHashTablePos(pattern);
			if (pos == -1) {
				is_success = hash_test.SetHashTable(pattern);
				long pos = hash_test.GetHashTablePos(pattern);
				myList[pos].insertList(i, j);
				//cout << pattern << "SUCCESS��LOCATION��" << hash_test.testid << endl;
			} else {
				myList[pos].insertList(i, j);
				//cout << "add " << pattern << " into location: " << pos << endl;
			}
		}
		cout << i << "/1000000 has completed!" << endl;
	}
	system("pause");

	//////////////////////////////////////////////////////////////////////////     
	// ���Ҳ���     
	char buff[6] = "TTAGC";
	pos = hash_test.GetHashTablePos(buff);
	pos != -1 ? cout << "���Ҳ����ַ�����" << buff << " ��ɢ��λ�ã�" << pos << endl : cout << buff << "���ڳ�ͻ��" << endl;
	myList[pos].outputList();
	int i;
	cin >> i;
	system("pause");
	return 0;
}