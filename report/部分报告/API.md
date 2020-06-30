#### 一、实现功能

API模块整合了更底层的Record Manager、Index Manager、Catalog Manager，以实现Interpreter层需要的更为复杂的功能。作为数据库系统的核心，API模块提供给Interpreter的接口包括：创建表、删除表、创建表、删除表、查询记录、插入记录、删除记录。API模块综合调度3个Manager；从Catalog Manager获取表/数据库的信息，进一步调用Record Manager和Index Manager，完成记录和索引的更新或查询。

#### 二、程序说明

##### API模块

```c++
class API
{
private:
	/*Internal function members below*/
	/*一些内部函数，不应该被interpreter调用！
	可能会被API本身、catalog manager、record manager、index manager模块调用
	没有列全，用到时自行补足即可*/
	//To be continued.

	//tableName:表名称
	//recordContent:记录record中实际内容
	//recordResult:用于存储一条record内容的指针
	//将recordContent中的内容转为一个char*指向的内容，之后recordResult指针会作为其它函数参数
	void recordStringGet(string tableName, vector<string>* recordContent, char* recordResult);
public:
	/*下面3个指针在main模块中才初始化，避免和3个manager产生冲突*/
	//指向全局catalog manager的指针
	CatalogManager *cm;
	//指向全局record manager的指针
	RecordManager *rm;
	//指向全局index manager的指针
	IndexManager *im;

	/*外部接口*/
	API() {};
	~API() {};
	//创建表
	bool createTable(std::string table_name, Attribute attribute, int primary, Index index);
	//删除表
	bool dropTable(std::string table_name);
	//创建索引
	void createIndex(string index_name, string table_name, string attribute_name);
	//删除索引
	void dropIndex(string table_name, string index_name);
	//展示记录 -- 无条件
	int showRecord(string table_name, vector<string>* attribute_names);
	//展示记录 -- 有条件
	int showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions);
	//插入记录
	void insertRecord(string table_name, vector<string>* record_content);
	//删除记录 -- 无条件
	int deleteRecord(string table_name);
	//删除记录 -- 有条件
	int deleteRecord(string table_name, vector<Condition>* conditions);
	
	//获取recordSize的函数
	int recordSizeGet(string tableName);
	//获取表中SingleAttribute的函数
	int attributeGet(string tableName, vector<SingleAttribute> *attributeVector);


	/*Internal function members below*/
	/*一些内部函数，不应该被interpreter调用！
	可能会被API本身、catalog manager、record manager、index manager模块调用
	没有列全，用到时自行补足即可*/
	//内部函数，插入**单个数据**对应的索引
	void insertIndex(string table_name, string index_name, char* key_addr, int type, int blockID);
	//内部函数，删除**单个数据**对应的索引
	void deleteIndex(string table_name, string index_name, char* key_addr, int type);
	//内部函数，删除给定地址开始的**一条记录**对应的索引
	void deleteRecordIndex(string table_name, char* record_begin, int record_size, Attribute attributes);
	//内部函数，插入给定地址开始的**一条记录**对应的索引
	void insertRecordIndex(string table_name, char* record_begin, int record_size, Attribute attributes, int blockID);
	//获得所有index的信息：<index文件名，index的type> -- 在Index_Manager.cpp中用于初始化所有index文件
	vector<pair<string, int> > allIndexInfoGet();
	//To be continued.
};
```

**成员变量**

+ `cm`：Catalog Manager的指针
+ `rm`：Record Manager的指针
+ `im`：Index Manager的指针

**成员函数**

+ `bool createTable(std::string table_name, Attribute attribute, int primary, Index index)`

  **创建表**接口；创建名为`table_name`的表，其属性保存在`attribute`结构中，主键指定为第`primary`个，并传入一个空的`index`结构。内部依次调用Catalog Manager和Record Manager的建表接口，实现表的创建。

+ `bool dropTable(std::string table_name)`

  **删除表**接口；删除名为`table_name`的表。内部先删除表的所有索引，再依次调用Catalog Manager和Record Manager的删除表接口，实现表的删除。

+ `void createIndex(string index_name, string table_name, string attribute_name)`

  **创建索引**接口；在`table_name`表的`attribute_name`属性上创建名为`index_name`的索引（**注意：允许不同表拥有同名索引**）。内部先调用Catalog Manager的`CreateIndex()`在catalog上记录新建的索引，再利用Record Manager的`createIndex()`新建索引文件，之后调用Index Manager的`createIndex()`创建相应的B+树，最后通过`RecordManager::indexRecordAllAlreadyInsert()`将已经存在的所有记录对应的（键值，blockID）插入到B+树中。

+ `void dropIndex(string table_name, string index_name)`

  **删除索引**接口；删除`table_name`表上的`index_name`索引。内部先调用Catalog Manager的`DropIndex()`在catalog上删除表的索引信息，随后依次调用Record Manager、Index Manager的`dropIndex()`接口，删除磁盘以及内存中的索引文件/B+树。

+ `int showRecord(string table_name, vector<string>* attribute_names)`

  **无条件查询记录**接口（即没有`where`语句）；将`table_name`表的**所有记录**投影到`attribute_names`上进行打印。实际上会交由有条件查询记录接口进行查询。

+ `int showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions)`

  **有条件查询记录**接口（即有`where`语句）；将`table_name`表中所有符合`conditions`中所有条件的记录，投影到`attribute_names`上进行打印（注意：**只支持`and`连接的条件**）。如果`conditions`的某个条件为**等值判定**，且判定属性已经建立了索引（那就意味着如果存在符合条件的记录，该记录必然是唯一的），则会利用索引直接找到该记录所在的块，再呈递给Record Manager加速查询；否则Record Manager需要遍历磁盘文件的每个块找到所有符合条件的记录。

+ `void insertRecord(string table_name, vector<string>* record_content)`

  **插入记录**接口；在`table_name`表中插入一条记录，记录内容由`record_content`指向的`std::vector`记录（依次保存每个属性的字符串值）。内部先处理`record_content`，将`int`、`float`类型的数据的字符串转换成二进制数据，暂时存放在内存的`recordString`（记录插入的一条记录）中。随后调用Record Manager将`recordString`写到缓冲池中，最终记录到磁盘上。此外，如果该表建立了索引，则需要向索引文件插入相应的（键值，blockID）以更新索引。

+ `int deleteRecord(string table_name)`

  **无条件删除记录**接口（即没有`where`语句）；删除`table_name`表的**所有记录**。实际上会交由有条件删除记录接口进行删除。

+ `int deleteRecord(string table_name, vector<Condition>* conditions)`

  **有条件删除记录**接口（即有`where`语句）；删除`table_name`表中同时所有满足`conditions`中每个条件的记录（注意：**只支持`and`连接的条件**）。如果`conditions`的某个条件为**等值判定**，且判定属性已经建立了索引（那就意味着如果存在符合条件的记录，该记录必然是唯一的），则会利用索引直接找到该记录所在的块，再呈递给Record Manager加速删除；否则Record Manager需要遍历磁盘文件的每个块找到所有符合条件的记录并删除。