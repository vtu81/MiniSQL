# Record Manager
## RecordManager类的构成
## public：
>
```
//@tablename:table名称
int createTable(string tablename);
```
- 建立table时调用，创建用于储存table的文件


```
//@tablename:table名称
int dropTable(string tablename);
```
- 删除table时调用，将buffer中的block全部output并删除对应文件


```
//@indexname:index名称
int createIndex(string indexname);
```
- 建立索引时调用，创建用于存储索引的文件


```
//@indexname:索引名称
int dropIndex(string indexname);
```
- 删除索引时调用，将buffer中的block全部output并删除对应文件


```
//@tablename:待插入的table名称
//@record：指向待插入record的指针
//@recordSize:待插入record的大小
int recordInsert(string tablename, char* record, int recordSize);
```
- 插入一条record，将record写入buffer并设置为dirty


```
//@tableName:表名
//@attributeNameVector:select后的attribute
//@conditionVector:where后的条件
int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector);
```
- 在表中查找并输出符合要求的结果


```
//@tableName:表名
//@attributeNameVector:select后的attribute
//@conditionVector:where后的条件
int recordAllFind(string tableName, vector<Condition>* conditionVector);
```
- 在表中查找并返回符合要求的record数目


```
//@tableName:表名
//@conditionVector:where后的条件
int recordAllDelete(string tableName, vector<Condition>* conditionVector);
```
- 按条件删除部分record，并返回删除的record条数


```
//@tableName:表名
//@indexName:索引名
int indexRecordAllAlreadyInsert(string tableName, string indexName);
```
- 插入表中所有index，返回插入的条数


## private:

```
BufferManager *bm;
```
- 一个BufferManager的指针，用来调用BufferManager的函数


```
int recordBlockShow(string tableFileName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID);
```
- 被recordAllShow（）调用，输出一个block内的查询结果


```
int recordBlockFind(string tableName, vector<Condition>* conditionVector, int pageID);
```
- 被recordAllFind()调用，返回一个block中符合查询条件的record数


```
int recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID);
```
- 被recordAllDelete()调用，删除block中符合要求的record并返回删除的数目


```
int indexRecordBlockAlreadyInsert(string tableName, string indexName, int blockID);
```
- 向一个block中插入index


```
bool recordConditionFit(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<Condition>* conditionVector);
```
- 判断一条record是否符合conditionVector中的一系列条件。如果全部符合条件，返回true,否则返回false


```
bool contentConditionFit(char* content, int type, Condition* condition);
```
- 判断一条record的某个attribute是否符合condition。符合则返回true,不符合则返回false


```
void recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector);
```
- 输出record(查询结果)


```
void contentPrint(char* content, int type);
```
- 被recordPrint()d=调用，逐个attribute输出record


```
char* findFirstUsableBlock(string tablename);
```
- 返回表中还未使用部分的开头


```
string getTableFileName(string tablename);
string getIndexFileName(string indexname);
```
- 分别根据表名、索引名获取对应的文件名称