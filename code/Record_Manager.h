#pragma once
#ifndef _RECORD_MANAGER_H_
#define _RECORD_MANAGER_H_
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include"Buffer_Manager.h"
#include"Condition.h"
#include"Attribute.h"
using namespace std;

class RecordManager {
public:
	int createTable(string tablename);
	int dropTable(string tablename);
	int createIndex(string indexname);
	int dropIndex(string indexname);
	int recordInsert(string tablename, char* record, int recordSize);
	//查询部分，要用到API和condition，attribute
	int recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector);
	//int recordBlockShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int blockOffset);
	int recordAllFind(string tableName, vector<Condition>* conditionVector);
	int recordAllDelete(string tableName, vector<Condition>* conditionVector);
	//int recordBlockDelete(string tableName, vector<Condition>* conditionVector, int blockOffset);
	int indexRecordAllAlreadyInsert(string tableName, string indexName);


private:
	BufferManager *bm;
	int recordBlockShow(string tableFileName, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID);
	int recordBlockFind(string tableName, vector<Condition>* conditionVector, int pageID);
	int recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID);
	int indexRecordBlockAlreadyInsert(string tableName, string indexName, int blockID);

	bool recordConditionFit(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<Condition>* conditionVector);
	bool contentConditionFit(char* content, int type, Condition* condition);

	void recordPrint(char* recordBegin, int recordSize, vector<Attribute>* attributeVector, vector<string> *attributeNameVector);
	void contentPrint(char* content, int type);

	char* findFirstUsableBlock(string tablename);
	int findContentBegin(char* block_content);
	string getTableFileName(string tablename);
	string getIndexFileName(string indexname);
};





#endif // !_RECORD_MANAGER_H_
