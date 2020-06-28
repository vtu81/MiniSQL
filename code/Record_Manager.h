#pragma once
#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_
#define _CRT_SECURE_NO_WARNINGS

#include<iostream>
#include<string>
#include<vector>
#include"Buffer_Manager.h"
#include"Condition.h"
#include"SingleAttribute.h"
#include"Catalog_Manager.h"
#include"API.h"
#include"basic.h"

#define RECORDFILEPATH "./database/record/"
#define INDEXFILEPATH "./database/index/"

using namespace std;

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

	int recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID);

	//检查一条record是否符合conditionVector中的一系列条件
	//如果全部符合条件，返回true,否则返回false
	//@recordBegin:指向record记录开始部分的指针
	//@recordSize:record的大小
	//@attributeVector:指向一个包含record所有attribute的vector
	//@conditionVector:指向一个包含所有condition的vector
	bool recordConditionFit(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<Condition>* conditionVector);

private:
	int recordBlockShow(string tableFileName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID);
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


#endif // !_RECORD_MANAGER_H_
