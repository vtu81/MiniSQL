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
    //执行语句s
    int interpreter(string s);
    Interpreter() {};
    ~Interpreter() {};

private:
    string getWord (string s, int& index);
    string getWord_no_symbol (string s, int& index);
    //string转int，如果超出范围则返回0
    int string2num(string s);
};
#endif;