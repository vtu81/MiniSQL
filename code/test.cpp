#include<iostream>
#include<fstream>
#include<string>
#include <time.h>
#include"Interpreter.h"
#include "API.h"
#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"
using namespace std;

BufferManager buffer_manager;
API api;
CatalogManager catalog_manager;
RecordManager record_manager;
IndexManager index_manager(&api);

int main()
{

	api.cm = &catalog_manager;
	api.rm = &record_manager;
	api.im = &index_manager;
	record_manager.api = &api;
	record_manager.bm = &buffer_manager;
	record_manager.cm = &catalog_manager;

	Interpreter s;
	s.api = &api;
	string query="",temp="";
	cout << "-------------MiniSQL interface-------------" << endl;

	while (1)
	{
		std::cout << "MiniSQL> " ;
		streambuf* orig_buf = cout.rdbuf();
		std::cout.rdbuf(NULL); // 抑制cout的数据，忽略过程中的输出信息
		clock_t start = clock();

		getline(cin, temp);//可直接输入execfile test.txt;
		query = query + temp;
		int pos = query.find(";");
		if (pos != query.npos)
		{
			query.erase(pos);
			//返回1代表成功执行，0代表出现bug，2代表退出SQL
			int condition=s.interpreter(query);
			std::cout.rdbuf(orig_buf); // 取消输出的抑制
			if (condition == 0)
				cout << "Error!" << endl;
			else if (condition == 2)
			{
				cout << "Exit program!" << endl;
				break;
			}
			query = "";
		}
		// 用时毫秒
		std::cout << "文件执行成功，耗时：" << (clock() - start) * 1.0 << "s" <<  std::endl;
		std::cout << "################################" << std::endl;
	}
	int i;
	cin >> i;
	return 0;
}
