#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "Interpreter.h"
#include "API.h"
#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"
using namespace std;

BufferManager buffer_manager;
API api;
CatalogManager catalog_manager(buffer_manager);
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

	index_manager.init();

	Interpreter s;
	s.api = &api;
	string query="",temp="";
	clock_t begin, end;
	cout << "-------------MiniSQL interface-------------" << endl;
	while (1)
	{
		cout << "MiniSQL> " ;
		getline(cin, temp);//可直接输入execfile test.txt;
		query = query + temp;
		int pos = query.find(";");
		if (pos != query.npos)
		{
			query.erase(pos);
			begin = clock();
			//返回1代表成功执行，0代表出现bug，2代表退出SQL
			int condition=s.interpreter(query);
			if (condition == 0)
			{
				// cout << "Error!" << endl;
			}
			else if (condition == 2)
			{
				cout << "Goodbye!" << endl;
				break;
			}
			else
			{
				end = clock();
				double elapsed_time = (double)(end - begin) / CLOCKS_PER_SEC;
				printf("(%3.2f sec)\n", elapsed_time);
			}
			query = "";
		}
	}
	// int i;
	// cin >> i;
	return 0;
}
