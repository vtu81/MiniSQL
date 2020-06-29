# Record Manager
### 一、实现功能
Record Manager主要为API提供记录的插入、删除、查询功能的接口；Record Manager负责每个表的记录文件的创建、更新以及在磁盘上的保存。利用Buffer Manager，Record Manager可以有效地通过较少的磁盘访问进行尽可能多的记录读写。
### 二、程序说明
Record Manager模块插入、删除、查询功能实现相关的代码封装在RecordManager类中，对应的代码文件是“RecordManager.h”和“RecordManager.cpp”。在`RecordManager`类中还定义了`BufferManager`、`API`、`CatalogManager`类的指针以便于调用其中的一些函数。为了判断记录是否符合条件定义了`Condition`类，其中提供了对记录进行判断的接口。

RecordManager 类

```c++
class RecordManager {
public:
	BufferManager *bm;
	CatalogManager *cm;
	API *api;

	string getTableFileName(string tablename);
	string getIndexFileName(string tablename, string indexname);
	//创建table时调用
	//创建存储table对应的文件
	//@tablename:table名称
	int createTable(string tablename);
	//删除table时调用
	//output所有buffer中的block并删除磁盘文件
	//@tablename:table名称
	int dropTable(string tablename);
	//创建index调用
	//创建存储index的对应文件
	//@indexname:index名称
	int createIndex(string tableName, string indexname);
	//删除index时调用
	//output所有buffer中的block并删除磁盘文件
	//@indexname:索引名称
	int dropIndex(string tableName, string indexname);

	//插入一条record
	//将record写入buffer中的page并设置page为dirty
	//@tablename:待插入的table名称
	//@record：指向待插入record的指针
	//@recordSize:待插入record的大小
	int recordInsert(string tablename, char* record, int recordSize);

	//根据条件查找record并显示
	//@tableName:表名
	//@attributeNameVector:select后的attribute
	//@conditionVector:where后的条件
	int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector);

	//找出符合条件的record条数并返回
	//@tableName:表名
	//@attributeNameVector:select后的attribute
	//@conditionVector:where后的条件
	int recordAllFind(string tableName, vector<Condition>* conditionVector);
	//删除符合要求的record，返回值为删除的record条数
	//@tableName:表名
	//@conditionVector:where后的条件
	int recordAllDelete(string tableName, vector<Condition>* conditionVector);
	//insert表中的所有index,返回insert的条数
	//@tableName:表名
	//@indexName:索引名
	int indexRecordAllAlreadyInsert(string tableName, string indexName);

    //删除一个block中符合要求的记录
    //@tableName:表名
    //@conditionVector:
	int recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID);

	//检查一条record是否符合conditionVector中的一系列条件
	//如果全部符合条件，返回true,否则返回false
	//@recordBegin:指向record记录开始部分的指针
	//@recordSize:record的大小
	//@attributeVector:指向一个包含record所有attribute的vector
	//@conditionVector:指向一个包含所有condition的vector
	bool recordConditionFit(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<Condition>* conditionVector);
	int recordBlockShow(string table_name, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID);
private:
	
	int recordBlockFind(string tableName, vector<Condition>* conditionVector, int pageID);
	int indexRecordBlockAlreadyInsert(string tableName, string indexName, int blockID);

	//检查一条record的某个attribute是否符合condition
	//符合返回true,不符合返回false
	//@content:指向record中某一attribute
	//@type:attribute的类型，-1表示float,0表示int,其他数字表示string
	//@condition:指向某一个Condition类，即某一个条件
	bool contentConditionFit(char* content, int type, Condition* condition);

	void recordPrint(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<string> *attributeNameVector);
	void contentPrint(char* content, int type);

	char* findFirstUsableBlock(string tablename);
	int findContentBegin(string table_name, char* block_content, int recordSize);
	bool noRecord(string table_name, char* recordBegin);
};
```
**成员变量**

+ `bm`：指向（全局共用的一个）Buffer Manager的指针
+ `cm`：指向（全局共用的一个）Catalog Manager的指针
+ `api`：指向（全局共用的一个）API的指针

**成员函数**

- ```getTableFileName(string tablename)```

  根据table名称得到table文件的路径并返回

- ```getIndexFileName(string tablename, string indexname)```

  根据index名称得到index文件的路径并返回

- ```createTable(string tablename)```

  创建表格时调用，在./database/record目录下生成对应的文件

- ```dropTable(string tablename)```

  删除table文件时调用，输出buffer pool中所有该table的记录文件的page并删除磁盘文件

- ```createIndex(string tableName, string indexname)```

  创建索引时调用，在./database/index下创建对应的储存索引的文件

- ```dropIndex(string tableName, string indexname)```

  删除index时调用，输出buffer pool中所有该index文件的page并删除磁盘文件

- ```recordInsert(string tablename, char* record, int recordSize)```

  给定表名、指向一条记录的指针、记录大小，将给定的记录插入表中

- ```recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector)```

  给定需要显示的属性名和查询条件，进行查询并输出结果

- ```recordAllFind(string tableName, vector<Condition>* conditionVector)```

  在指定的表中进行查找并返回符合条件的记录条数

- ```int recordAllDelete(string tableName, vector<Condition>* conditionVector)```

  删除表中符合条件的所有记录

- ```indexRecordAllAlreadyInsert(string tableName, string indexName)```


  查询表中已经插入的Index数目并返回
- ```recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID)```

  删除指定block中符合条件的记录

- ```recordConditionFit(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<Condition>* conditionVector)```

  判断一条record是否符合给定的条件

- ```recordBlockShow(string table_name, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID)```

  在指定的block中查询符合条件的记录并显示




