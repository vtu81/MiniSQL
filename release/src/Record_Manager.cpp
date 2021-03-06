#include"Record_Manager.h"
using namespace std;

int RecordManager::createTable(string tablename) {
	string filename = getTableFileName(tablename);
	FILE* fp;
	fp = fopen(filename.c_str(), "w+");
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
	string TableFileName = getTableFileName(tablename);
	// int i = 0;
	// int PageID = bm->fetchPageID(TableFileName.c_str(), i);
	// while (PageID != PAGENOTEXIST) {
	// 	//这里把所有block都output一遍
	// 	bm->outputPage(PageID);
	// 	i++;
	// 	PageID = bm->fetchPageID(TableFileName.c_str(), i);
	// }
	bm->outputFile(TableFileName);
	remove(TableFileName.c_str());
	return 1;
}

int RecordManager::createIndex(string tableName, string indexname) {
	string filename = getIndexFileName(tableName, indexname);
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

int RecordManager::dropIndex(string tableName, string indexname) {
	string IndexFileName = getIndexFileName(tableName, indexname);
	// int i = 0;
	// int PageID = bm->fetchPageID(IndexFileName.c_str(), i);
	// while (PageID != PAGENOTEXIST) {
	// 	//这里把所有block都output一遍
	// 	bm->outputPage(PageID);
	// 	i++;
	// 	PageID = bm->fetchPageID(IndexFileName.c_str(), i);
	// }
	bm->outputFile(IndexFileName);
	remove(IndexFileName.c_str());
	return 1;
}

string RecordManager::getTableFileName(string tablename) {
	string A;
	A = RECORDFILEPATH + tablename;
	return A;
}

string RecordManager::getIndexFileName(string tablename, string indexname) {
	string A;
	A = INDEXFILEPATH + tablename + "_" + indexname;
	return A;
}

int RecordManager::recordInsert(string tablename, char* record, int recordSize) {
	int i = 0;
	string TableFileName = getTableFileName(tablename);
	char * firstUsableBlock = bm->fetchPage(TableFileName, i);
	int contentBegin = findContentBegin(tablename, firstUsableBlock, recordSize);
	while (contentBegin == -1 || contentBegin + recordSize > PAGESIZE) {
		i++;
		firstUsableBlock = bm->fetchPage(TableFileName, i);
		contentBegin = findContentBegin(tablename, firstUsableBlock, recordSize);
	}
	memcpy(firstUsableBlock + contentBegin, record, recordSize);
	bm->markPageDirty(i);
	return i;

}

int RecordManager::recordAllShow(string tableName, vector<string>* attributeNameVector, vector<Condition>* conditionVector) {
	string TableFileName = getTableFileName(tableName);
	int count = 0;
	int i = 0;
	char* contentBegin = bm->fetchPage(TableFileName.c_str(), 0);
	int recordSize = api->recordSizeGet(tableName);

	while (!noRecord(tableName, contentBegin)) {
		count += recordBlockShow(tableName, attributeNameVector, conditionVector, i);
		i++;
		contentBegin = bm->fetchPage(TableFileName.c_str(), count);
	}
	return count;

}

int RecordManager::recordBlockShow(string table_name, vector<string>* attributeNameVector, vector<Condition>* conditionVector, int pageID) {
	string tableFileName = getTableFileName(table_name);
	int count = 0;
	char* recordBegin = bm->fetchPage(tableFileName, pageID);
	if (noRecord(table_name, recordBegin)) {
		return -1;
		cout << "NO record" << endl;
	}
	else {
		vector<SingleAttribute> attributeVector;
		int recordSize = api->recordSizeGet(table_name);
		//将表中的attribute拷贝到attributeVector中
		api->attributeGet(table_name, &attributeVector);
		int usingSize = findContentBegin(table_name, recordBegin, recordSize);
		char* usingBegin = bm->fetchPage(tableFileName, pageID);
		while (recordBegin - usingBegin < usingSize) {
			if (recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector)) {
				count++;
				recordPrint(recordBegin, recordSize, &attributeVector, attributeNameVector);
				std::cout << std::endl;
			}
			recordBegin += recordSize;
		}
	}
	return count;
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
		std::cout << tmp << "\t";
	}
	else if (type == SingleAttribute::TYPE_FLOAT)
	{
		float tmp = *((float *)content);
		std::cout << tmp << "\t";
	}
	else
	{
		string tmp = content;
		std::cout << tmp << "\t";
	}
}

int RecordManager::recordAllFind(string tableName, vector<Condition>* conditionVector) {
	int i = 0;
	int count = 0;
	string TableFileName = getTableFileName(tableName);
	char* recordBegin = bm->fetchPage(TableFileName, i);
	while (!noRecord(tableName, recordBegin)) {
		count += recordBlockFind(tableName, conditionVector, i);
		i++;
		recordBegin = bm->fetchPage(TableFileName, i);
	}
	return count;
}

int RecordManager::recordBlockFind(string tableName, vector<Condition>* conditionVector, int pageID) {
	int count = 0;
	string tableFileName = getTableFileName(tableName);
	int recordSize = api->recordSizeGet(tableName);
	char* recordBegin = bm->fetchPage(tableFileName, pageID);
	char* usingBegin = bm->fetchPage(tableFileName, pageID);
	int usingSize = findContentBegin(tableName, usingBegin, recordSize);
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
	int count = 0;
	string TableFileName = getTableFileName(tableName);
	char* recordBegin = bm->fetchPage(TableFileName, i);
	while (!noRecord(tableName, recordBegin)) {
		count+=recordBlockDelete(tableName, conditionVector, i);
		i++;
		recordBegin = bm->fetchPage(TableFileName, i);
	}
	return count;
}

int RecordManager::recordBlockDelete(string tableName, vector<Condition>* conditionVector, int pageID) {
	string tableFileName = getTableFileName(tableName);
	int recordSize = api->recordSizeGet(tableName);
	int count = 0;
	char* recordBegin = bm->fetchPage(tableFileName, pageID);
	char* usingBegin = bm->fetchPage(tableFileName, pageID);
	int usingSize = findContentBegin(tableName, usingBegin, recordSize);
	vector<SingleAttribute> attributeVector;

	api->attributeGet(tableName, &attributeVector);
	Attribute tableAttribute = cm->GetAttribute(tableName);
	while (recordBegin - usingBegin < usingSize)
	{
		if (!noRecord(tableName, recordBegin) && recordConditionFit(recordBegin, recordSize, &attributeVector, conditionVector))
		{
			count++;
			api->deleteRecordIndex(tableName, recordBegin, recordSize, tableAttribute);
			int i = 0;
			for (i = 0; i + recordBegin + recordSize < usingBegin + usingSize; i++)
			{
				recordBegin[i] = recordBegin[i + recordSize];
			}
			memset(recordBegin + i, 0, recordSize);
			bm->markPageDirty(pageID);
		}
		else recordBegin += recordSize;
	}
	return count;
}

int RecordManager::indexRecordAllAlreadyInsert(string tableName, string indexName) {
	string tableFileName = getTableFileName(tableName.c_str());
	int count = 0;
	int i = 0;
	char* recordBegin = bm->fetchPage(tableFileName, i);
	while (recordBegin[0] != '\0')
	{
		int recordBlockNum = indexRecordBlockAlreadyInsert(tableName, indexName, i);
		count += recordBlockNum;
		i++;
		recordBegin = bm->fetchPage(tableFileName, i);
	}

	return count;;
}

int RecordManager::indexRecordBlockAlreadyInsert(string tableName, string indexName, int blockID) {
	int count = 0;
	string tableFileName = getTableFileName(tableName);
	int recordSize = api->recordSizeGet(tableName);
	char* recordBegin = bm->fetchPage(tableFileName, blockID);
	char* usingBegin = bm->fetchPage(tableFileName, blockID);
	int usingSize = findContentBegin(tableName, usingBegin, recordSize);
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
				api->insertRecordIndex(tableName, contentBegin, recordSize, tableAttribute, blockID);
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
	string TableFileName = getTableFileName(tablename);
	int i = 0;
	char* beginBlock = bm->fetchPage(TableFileName, i);
	while (beginBlock[PAGESIZE - 1] != '\0') {
		i++;
		beginBlock = bm->fetchPage(TableFileName, i);
	}
	return beginBlock;
}

int RecordManager::findContentBegin(string table_name, char* block_content, int recordSize) {
	int i = 0;
	while (!noRecord(table_name, block_content + i) && i < PAGESIZE) {
		i += recordSize;
	}
	if (i >= PAGESIZE) {
		return -1;
	}
	else {
		return i;
	}
}

bool RecordManager::noRecord(string table_name, char* recordBegin) {
	bool noRecord = true;
	Attribute table_attr = cm->GetAttribute(table_name);
	int firstAttrSize;
	if (table_attr.type[0] == -1) {
		firstAttrSize = sizeof(int);
	}
	else if (table_attr.type[0] == 0) {
		firstAttrSize = sizeof(float);
	}
	else {
		firstAttrSize = sizeof(char)*table_attr.type[0];
	}
	for (int i = 0; i < firstAttrSize; i++) {
		if (recordBegin[i] != '\0') {
			noRecord = false;
			break;
		}
	}
	return noRecord;
}
