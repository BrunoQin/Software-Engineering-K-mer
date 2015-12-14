#pragma once
#include <stdio.h>
#include "strNode.h"

using namespace std;

class strList {
private:
	strNode *head;
public:
	strList() { head = NULL; }
	void insertList(long fileNum, int location);
	void outputList();
	strNode *getHead() { return head; }
};

void strList::insertList(long fileNum, int location) {
	strNode *p, *s; 
	s = (strNode*)new(strNode); 
	s->fileNum = fileNum; 
	s->location = location;
	s->next = NULL;
	p = head;
	if (head == NULL) //若是空表，使b作为第一个结点
	{
		head = s;
		s->next = NULL;
	}
	else {
		while (p->next != NULL) {
			p = p->next;
		}
		p->next = s;
	}
}

void strList::outputList() {
	strNode *p;
	p = head;
	if (p != NULL) {
		while (p != NULL) {
			cout << p->fileNum << "    " << p->location << "\n";
			p = p->next;
		}
	}
	else {
		cout << 0;
	}
	
}
