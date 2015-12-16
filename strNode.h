#pragma once

struct strNode{
public:
	long fileNum;
	int location;
	strNode(long fileName, int location) :
		fileNum(fileName),
		location(location) {

	}
};
