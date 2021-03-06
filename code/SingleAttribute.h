#pragma once
#ifndef _SINGLEATTRIBUTE_H_
#define _SINGLEATTRIBUTE_H_

#include <string>
#include <iostream>
using namespace std;

class SingleAttribute
{
public:
	//属性名
	string name;
	//属性类型-1->int,0->float,长度为n的string=n
	int type;
	//是否为unique
	bool ifUnique;
	//初始值为""，即没有任何index
	string index;      
	//构造函数
	SingleAttribute(string n, int t, bool i);

public:
	//定义不同类型对应的数
	int static const TYPE_FLOAT = 0;
	int static const TYPE_INT = -1;
	//返回attribute类型
	int getType();
	//返回attribute大小
	int getSize();
	//设置index名称
	void SetIndex(string indexName);
	//返回index名称
	string indexNameGet();

};

#endif
