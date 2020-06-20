#include<iostream>
#include<fstream>
#include<string>
#include"Interpreter.h"
using namespace std;

int main()
{
	//解释器
	Interpreter s;
	string query="",temp="";
	/*query = "create table school (sid char(5), age int, primary key (sid) )";
	s.interpreter(query);
	query="create index index_sid on school (sid) ";
	s.interpreter(query);
	query = "drop table school ";
	s.interpreter(query);
	query = "drop index index_sid";
	s.interpreter(query); */
/*	ifstream in("test.txt");
	while (!in.eof())
	{
		getline(in, temp);
		query = query + temp;
		int pos = query.find(";");
		if (pos != query.npos)
		{
			query.erase(pos);
			cout << query << endl;
			s.interpreter(query);
			query = "";
		}
	}*/
	//执行SQL脚本文件
	/*
	query = "execfile test.txt";
	s.interpreter(query);
	*/
	cout << "-------------MiniSQL interface-------------" << endl;
	while (1)
	{
		getline(cin, temp);//可直接输入execfile test.txt;
		query = query + temp;
		int pos = query.find(";");
		if (pos != query.npos)
		{
			query.erase(pos);
			//返回1代表成功执行，0代表出现bug，2代表退出SQL
			int condition=s.interpreter(query);
			if (condition == 0)
				cout << "Error!" << endl;
			else if (condition == 2)
			{
				cout << "Exit program!" << endl;
				break;
			}
			query = "";
		}
	}
	return 0;
}
