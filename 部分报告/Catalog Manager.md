## Catalog Manager

Catalog Manager主要记录了所有表的定义信息，将每一个表信息按一定格式记录在文件中，并提供方位上述信息的接口，同时Catalog Manager调用Buffer Manager实际完成完成表的创建、删除等等相关功能。



Catalog Manager模块将表的创建、删除功能实现的相关代码封装在`CatalogManager`类中，相对应的代码"CatalogManager.h"和“CatalogManager.cpp”中，此外还提供了提供了一些方便API调用的接口。

```C++
class CatalogManager{
public:
	BufferManager BM;
	/*
	@parammeter：table_name,attribute_name,primary_key,index
	Function：insert a table in catalog file
	Output：if repeat, output error table_not_exist
	*/
    void CreateTable(std::string t_name, Attribute attribute, int primary, Index index);
	/*
    @parammeter：table_name
    Function：delete a table in catalog file
    Output：if no such table, output error table_not_exist
	*/
    void DropTable(std::string t_name);
	/*
	@parammeter：table_name,attribute_name,index
	Function：Create a index for an atrribute on a table
	Output：if no such table，output error table_not_exist.
	if no such attribute，output error:attribute_not_exist。
	if index repeat，output error index_exist
	*/
	void CreateIndex(std::string t_name, std::string a_name, std::string i_name);
	/*
	@parammeter：table_name,index
	Function：find attribute through index
	Output：if no such table，output error table_not_exist
			if no such index，output error index_not_exist
	*/
	std::string IndextoAttr(std::string t_name, std::string i_name);
	/*
	@parammeter：table_name,index
	Function：Delete index of a table
	output：if no such table，output error table_not_exist
			if no such attribute, output error attribute_not_exist
			if no such index，output error index_not_exist
	*/
	void DropIndex(std::string t_name, std::string i_name);
    /*
	@parammeter：table_name
    Function：check if the table exists, ouput true for yes and false for no
	*/
    bool IsTable(std::string t_name);
    /*
	@parammeter：table_name, attribute_name
    Function：check if the attribute exists, true for yes and false for no
    Output：if no such attribute,output error table_not_exist
	*/
    bool IsAttribute(std::string t_name , std::string a_name);
    /*
	@parammeter：table_name
    Function：Get the attribute from a named table
    Ouput：if no such table，output error table_not_exist
	*/
    Attribute GetAttribute(std::string t_name);
    /*
	@parammeter：table_name
    Function：show table information
    output：if no such table，output error able_not_exist
	*/
    void PrintTable(std::string t_name);
	/*get the index of the table*/
	Index GetIndex(std::string t_name);
	/*Get all table name*/
	std::vector <std::string> GetAllTable();
private:
    /*tranform numer to string，bit is width of number*/
    std::string numtostr(int num,short bit);
    /*string to number*/
    int strtonum(std::string str);
    /*get the table name*/
    std::string GetTableName(std::string buffer,int start,int &rear);
    /*get the address of the table and return the block information */
    int GetTableAdress(std::string t_name,int &suitable_block);
    /*get the size of the file*/
    int fetchBlockNum(std::string t_name);
	/*get table information*/
	std::vector <std::string> tablelist;
	int tablenum = 0;
};
```

**成员变量**

+`BM`:Buffer Manager的指针

+`TableList`：用于记录已创建表的指针链表

+`tablenum`:用来记录现有的表的数量

**成员函数**

`void CreateTable(std::string t_name, Attribute attribute, int primary, Index index)`

创建一个新表记录，并通过调用Buffer Manager为其分配一个内存page，输入变量位**表名**、**列名**、**主键位置**、**索引**

`void DropTable(std::string t_name)`

根据输入的**表名**删掉对应的表的记录，并相应回收已分配的内存。

`void CreateIndex(std::string t_name, std::string a_name, std::string i_name)`

创建一个新的索引，输入分别为**表名**、**列名**、**索引名**

`std::string IndextoAttr(std::string t_name, std::string i_name)`

通过输入的**表名**、**索引名**搜索对应的一个列

`void DropIndex(std::string t_name, std::string i_name)`

通过输入的**表名**、**索引名**删除对应的一个列

`bool IsTable(std::string t_name)`

通过输入的**表名**判断是否已经有相同名字的表存在

`bool IsAttribute(std::string t_name , std::string a_name)`

通过输入的**表名**、**列名**判断是否已经有相同名字的列存在

`Attribute GetAttribute(std::string t_name)`

由输入的**表名**得到其中所有的列名

`void PrintTable(std::string t_name)`

由输入的**表名**得到该表的所有

`Index GetIndex(std::string t_name)`

由输入的**表名**的到该表的索引

`std::vector <std::string> GetAllTable()`

返回已有的所有表名

`std::string numtostr(int num,short bit)`

将数字转化成便于记录的字符串

`int strtonum(std::string str)`

将记录中的字符串转化成为数字

`std::string GetTableName(std::string buffer,int start,int &rear)`

通过输入的buffer指针和需要搜索的始末位置，从已开辟的内存page中得到所有记录中的表名



