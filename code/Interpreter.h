#ifndef _INTERPRETER_H_
#define _INTERPRETER_H_
#include <string>
#include <vector>
#include<algorithm>
#include "Attribute.h"

using namespace std;
class Interpreter
{
public:
	//API *api;
	string filename;
	//get the next word starting from index
	string getWord (string s,int& index);
	//ִ�����s
	int interpreter(string s);
	//stringתint�����������Χ�򷵻�0
	int string2num(string s);
	
	Interpreter() {};
	~Interpreter() {};
};
#endif;