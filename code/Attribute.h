#pragma once
#ifndef _ATTRIBUTE_H_
#define _ATTRIBUTE_H_

#include <string>
#include <iostream>
using namespace std;

class Attribute
{
public:
	string name;
	int type;                  //the type of the attribute,-1 represents float, 0 represents int, other positive integer represents char and the value is the number of char)
	bool ifUnique;
	string index;         // default value is "", representing no index
	Attribute(string n, int t, bool i);

public:
	int static const TYPE_FLOAT = -1;
	int static const TYPE_INT = 0;
	int getType();
	int getSize();
	string indexNameGet() { return index; }

};

#endif