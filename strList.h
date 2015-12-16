#pragma once
#include <stdio.h>
#include <vector>
#include "strNode.h"

using namespace std;

class strList {
private:
	vector<strNode> head;
public:
	void insertList(long fileNum, int location);
	void outputList();
};

void strList::insertList(long fileNum, int location) {
	strNode *s = new strNode(fileNum, location);
	head.push_back(*s);
	delete s;
}

void strList::outputList() {

	for (int i = 0; i < head.size(); i++) {
		cout << head[i].fileNum << "     " << head[i].location << endl;
	}
	
}
