#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <string>
#include <vector>
#include <fstream>
#include<algorithm>
#include "basic.h"
#include "API.h"
#include "Exception.h"
#include "Condition.h"

using namespace std;
class Interpreter
{
public:
	API *api;
	string filename;
	//get the next word starting from index
	//ִ�����s
	int interpreter(string s);
	Interpreter() {};
	~Interpreter() {};

private:
	string getWord(string s, int& index);
	string getWord_no_symbol(string s, int& index);
	//stringתint�����������Χ�򷵻�0
	int string2num(string s);
};
#endif