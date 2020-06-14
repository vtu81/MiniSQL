#pragma once
#ifndef _CONDITION_H_
#define _CONDITION_H_
#include <string>
#include <sstream>
using namespace std;

class Condition
{

public:
	//存储各种比较操作
	const static int OPERATOR_EQUAL = 0; // "="
	const static int OPERATOR_NOT_EQUAL = 1; // "<>"
	const static int OPERATOR_LESS = 2; // "<"
	const static int OPERATOR_MORE = 3; // ">"
	const static int OPERATOR_LESS_EQUAL = 4; // "<="
	const static int OPERATOR_MORE_EQUAL = 5; // ">="

	//构造函数
	Condition(string a, string v, int o);

	//待比较的attribute名称
	string attributeName;
	//待比较的值
	string value;
	//待比较的attribute类型
	int operate;

	//对不同类型的content重载，进行比较
	bool ifRight(int content);
	bool ifRight(float content);
	bool ifRight(string content);
};

#endif
