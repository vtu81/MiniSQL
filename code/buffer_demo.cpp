#include"Buffer_Manager.h"
using namespace std;

int main()
{
	//��ʼ��һ�����󣬿�������page_pool_size��ֵ
	BufferManager B;
	//��ʾ��buffer_demo.txt�ļ��У������һ����
	char *s = B.fetchPage("buffer_demo.txt", 0);
	//����buffer�е�λ��
	int a= B.fetchPageID("buffer_demo.txt", 0);
	cout << s << endl<<a;
	s[0] = 'I';
	s[1] = 'D';
	//д��ԭ�ļ�
	B.outputPage(0);
	return 0;
}
/*
ԭbuffer_demo�ļ���
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