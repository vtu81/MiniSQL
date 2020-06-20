#include"Record_Manager.h"
using namespace std;

int RecordManager::createTable(string tablename) {
	string filename=getTableFileName(tablename);
	FILE* fp; 
	fp=fopen(filename.c_str(),"w+");
	if (fp == NULL) {
		return -1;
	}
	else {
		//bm->fetchPage(filename.c_str(),0);
		fclose(fp);
		return 1;
	}
}

int RecordManager::dropTable(string tablename) {
	string TableFileName=getTableFileName(tablename);
	int i = 0;
	int PageID = bm->fetchPageID(TableFileName.c_str(),i);
	while (PageID != PAGENOTEXIST) {
		//这里把所有block都output一遍
		bm->outputPage(PageID);
		i++;
		PageID=bm->fetchPageID(TableFileName.c_str(),i);
	}
	remove(TableFileName.c_str());
}

int RecordManager::createIndex(string tableName,string indexname) {
	string filename = getIndexFileName(tableName,indexname);
	FILE* fp;
	fp = fopen(filename.c_str(), "w+");
	if (fp == NULL) {
		return -1;
	}
	else {
		//bm->fetchPage(filename.c_str(), 0);
		fclose(fp);
		return 1;
	}
}

int RecordManager::dropIndex(string tableName,string indexname) {
	string IndexFileName = getIndexFileName(tableName,indexname);
	int i = 0;
	int PageID = bm->fetchPageID(IndexFileName.c_str(), i);
	while (PageID != PAGENOTEXIST) {
		//这里把所有block都output一遍
		bm->outputPage(PageID);
		i++;
		PageID = bm->fetchPageID(IndexFileName.c_str(), i);
	}
	remove(IndexFileName.c_str());
}

string RecordManager::getTableFileName(string tablename) {
	string A;
	A = RECORDFILEPATH+ tablename;
	return A;
}

string RecordManager::getIndexFileName(string tablename,string indexname) {
	string A;
	A = INDEXFILEPATH + tablename+"_"+indexname;
	return A;
}

int RecordManager::recordInsert(string tablename, char* record, int recordSize) {
	int i = 0;
	string TableFileName = getTableFileName(tablename);
	char * firstUsableBlock=bm->fetchPage(TableFileName,i);
	int contentBegin = findContentBegin(firstUsableBlock,recordSize);
	while (contentBegin + recordSize > PAGESIZE) {
		i++;
		firstUsableBlock = bm->fetchPage(TableFileName, i);
		contentBegin = findContentBegin(firstUsableBlock,recordSize);
	}
	memcpy(firstUsableBlock+contentBegin, record, recordSize);
	bm->markPageDirty(i);

}

int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector) {
	string TableFileName = getTableFileName(tableName);
	int count = 0;
	char* contentBegin = bm->fetchPage(TableFileName.c_str(), 0);
	int recordSize = api->recordSizeGet(tableName);
	while (contentBegin[0] != '\0') {
		count++;
		contentBegin = bm->fetchPage(TableFileName.c_str(), count);
		recordBlockShow(tableName,attributeNameVector,conditionVector,count);
	}
	return count-1;

}

int RecordManager::recordBlockShow(string table_name, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID) {
	string tableFileName = getTableFileName(table_name);
	char* recordBegin = bm->fetchPage(tableFileName, pageID);
	if (recordBegin[0] = '\0') {
		return -1;
	}
	else {
		int count = 0;
		vector<SingleAttribute> attributeVector;
		int recordSize = api->recordSizeGet(table_name);
		//将表中的attribute拷贝到attributeVector中
		api->attributeGet(table_name, &attributeVector);
		int usingSize = findContentBegin(recordBegin,recordSize);
		char* usingBegin = bm->fetchPage(tableFileName, pageID);
		while (recordBegin - usingBegin < usingSize) {
			if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)) {
				count++;
				recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
				printf("\n");
			}
			recordBegin += recordSize;
		}
	}
	return count-1;
}

void RecordManager::recordPrint(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<string> *attributeNameVector)
{
	int type;
	string attributeName;
	int typeSize;
	char content[255];

	char *contentBegin = recordBegin;
	for (int i = 0; i < attributeVector->size(); i++)
	{
		type = (*attributeVector)[i].type;
		typeSize = (*attributeVector)[i].getSize();

		memset(content, 0, 255);

		memcpy(content, contentBegin, typeSize);

		for (int j = 0; j < (*attributeNameVector).size(); j++)
		{
			if ((*attributeNameVector)[j] == (*attributeVector)[i].name)
			{
				contentPrint(content, type);
				break;
			}
		}
		contentBegin += typeSize;
	}
}

void RecordManager::contentPrint(char* content, int type) {
	if (type == SingleAttribute::TYPE_INT)
	{
		int tmp = *((int *)content); 
		printf("%d ", tmp);
	}
	else if (type == SingleAttribute::TYPE_FLOAT)
	{
		float tmp = *((float *)content); 
		printf("%f ", tmp);
	}
	else
	{
		string tmp = content;
		printf("%s ", tmp.c_str());
	}
}

int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector) {
	int i = 0;
	string TableFileName = getTableFileName(tableName);
	char* recordBegin = bm->fetchPage(TableFileName,i);
	while (recordBegin[0] != '\0') {
		recordBlockFind(tableName, conditionVector, i);
		i++;
		recordBegin = bm->fetchPage(TableFileName, i);
	}
	return i-1;
}

int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, int pageID) {
	int count = 0;
	string tableFileName = getTableFileName(tableName);
	int recordSize = api->recordSizeGet(tableName);
	char* recordBegin = bm->fetchPage(tableFileName,pageID);
	char* usingBegin=bm->fetchPage(tableFileName, pageID);
	int usingSize = findContentBegin(usingBegin,recordSize);
	vector<SingleAttribute> attributeVector;
	api->attributeGet(tableName, &attributeVector);
	while (recordBegin - usingBegin < usingSize)
	{
		if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
		{
			count++;
		}
		recordBegin += recordSize;
	}

	return count;
}

int RecordManager::recordAllDelete(string tableName, vector<Condition>* conditionVector) {
	int i = 0;
	string TableFileName = getTableFileName(tableName);
	char* recordBegin = bm->fetchPage(TableFileName, i);
	while (recordBegin[0] != '\0') {
		recordBlockDelete(tableName, conditionVector, i);
		i++;
		recordBegin = bm->fetchPage(TableFileName, i);
	}
	return i-1;
}

int RecordManager::recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID) {
	string tableFileName = getTableFileName(tableName);
	int recordSize = api->recordSizeGet(tableName);
	int count = 0;
	char* recordBegin = bm->fetchPage(tableFileName, pageID);
	char* usingBegin = bm->fetchPage(tableFileName, pageID);
	int usingSize = findContentBegin(usingBegin,recordSize);
	vector<SingleAttribute> attributeVector;

	api->attributeGet(tableName, &attributeVector);
	Attribute tableAttribute = cm->GetAttribute(tableName);
	while (recordBegin - usingBegin < usingSize)
	{
		if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
		{
			count++;

			api->deleteRecordIndex(tableName, recordBegin, recordSize, tableAttribute);
			int i = 0;
			for (i = 0; i + recordSize + recordBegin - usingBegin< usingSize; i++)
			{
				recordBegin[i] = recordBegin[i + recordSize];
			}
			memset(recordBegin + i, 0, recordSize);
			bm->markPageDirty(pageID);
		}
		recordBegin += recordSize;
	}

	return count;
}

int RecordManager::indexRecordAllAlreadyInsert(string tableName, string indexName) {
	string tableFileName = getTableFileName(tableName.c_str());
	int count = 0;
	int i = 0;
	char* recordBegin = bm->fetchPage(tableFileName,i);
	while (recordBegin[0]!='\0')
	{
		int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, i);
		count += recordBlockNum;
		i++;
		recordBegin = bm->fetchPage(tableFileName, i);
	}

	return count;
}

int RecordManager::indexRecordBlockAlreadyInsert(string tableName, string indexName, int blockID) {
	int count = 0;
	string tableFileName = getTableFileName(tableName);
    int recordSize = api->recordSizeGet(tableName);
	char* recordBegin = bm->fetchPage(tableFileName, blockID);
	char* usingBegin = bm->fetchPage(tableFileName, blockID);
	int usingSize = findContentBegin(usingBegin,recordSize);
	vector<SingleAttribute> attributeVector;
	Attribute tableAttribute = cm->GetAttribute(tableName);
	
	api->attributeGet(tableName, &attributeVector);

	int type;
	int typeSize;
	char * contentBegin;

	while (recordBegin - usingBegin < usingSize)
	{
		contentBegin = recordBegin;
		for (int i = 0; i < attributeVector.size(); i++)
		{
			type = attributeVector[i].type;
			if (type == SingleAttribute::TYPE_FLOAT) typeSize = sizeof(float);
			else if (type == SingleAttribute::TYPE_INT) typeSize = sizeof(int);
			else typeSize = sizeof(char)*type;

			if (attributeVector[i].index == indexName)
			{
				//这里有一个insert index的函数，判断type在api中完成，然后再调用im函数进行insert
				api->insertRecordIndex(tableName,contentBegin,recordSize, tableAttribute,blockID);
				count++;
			}

			contentBegin += typeSize;
		}
		recordBegin += recordSize;
	}

	return count;
}

bool RecordManager::recordConditionFit(char* recordBegin, int recordSize, vector<SingleAttribute>* attributeVector, vector<Condition>* conditionVector) {
	if (conditionVector == NULL) {
		return true;
	}
	int type;
	string attributeName;
	int typeSize;
	char content[255];

	char *contentBegin = recordBegin;
	for (int i = 0; i < attributeVector->size(); i++)
	{
		type = (*attributeVector)[i].type;
		attributeName = (*attributeVector)[i].name;
		typeSize = (*attributeVector)[i].getSize();

		memset(content, 0, 255);
		memcpy(content, contentBegin, typeSize);
		for (int j = 0; j < (*conditionVector).size(); j++)
		{
			if ((*conditionVector)[j].attributeName == attributeName)
			{
				if (!contentConditionFit(content, type, &(*conditionVector)[j]))
				{
					return false;
				}
			}
		}

		contentBegin += typeSize;
	}
	return true;
}

bool RecordManager::contentConditionFit(char* content, int type, Condition* condition) {
	if (type == SingleAttribute::TYPE_INT)
	{
		int tmp = *((int *)content);   
		return condition->ifRight(tmp);
	}
	else if (type == SingleAttribute::TYPE_FLOAT)
	{
		float tmp = *((float *)content);  
		return condition->ifRight(tmp);
	}
	else
	{
		return condition->ifRight(content);
	}
	return true;
}

char* RecordManager::findFirstUsableBlock(string tablename) {
	string TableFileName=getTableFileName(tablename);
	int i=0;
	char* beginBlock=bm->fetchPage(TableFileName,i);
	while (beginBlock[PAGESIZE - 1] != '\0') {
		i++;
		beginBlock=bm->fetchPage(TableFileName,i);
	}
	return beginBlock;
}

int RecordManager::findContentBegin(char* block_content,int recordSize) {
	int i = 0;
	while (block_content[i] != '\0'&&i<PAGESIZE) {
		i+=recordSize;
	}
	if (i >= PAGESIZE) {
		return -1;
	}
	else {
		return i;
	}
}
