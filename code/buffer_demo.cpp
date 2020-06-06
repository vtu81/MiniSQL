#include"Buffer_Manager.h"
using namespace std;

int main()
{
	//初始化一个对象，可以设置page_pool_size的值
	BufferManager B;
	//表示从buffer_demo.txt文件中，读入第一个块
	char *s = B.fetchPage("buffer_demo.txt", 0);
	//返回buffer中的位置
	int a= B.fetchPageID("buffer_demo.txt", 0);
	cout << s << endl<<a;
	s[0] = 'I';
	s[1] = 'D';
	//写回原文件
	B.outputPage(0);
	return 0;
}
/*
原buffer_demo文件：
id                  int
emp_name            varchar
sex                 enum
age                 int
hire_date           date
post                varchar
post_comment        varchar
salary              double
office              int
depart_id           int      int
*/