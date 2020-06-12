#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include <string>
#include <vector>
#include "Attribute.h"
#include "BufferManager.h"
#include "IndexInfo.h"
using namespace std;
class CatalogManager {
public:
    BufferManager BM;
    CatalogManager();
    virtual ~CatalogManager();
    int AddIndex(string indexName,string tableName,string attributeName,int type);
    int RevokeIndex(string tableName,string AttributeName,string indexName);
    int FindTable(string tableName);
    int FindIndex(string indexName);
    int DropTable(string tableName);
    int DropIndex(string index);
    int DeleteValue(string tableName, int deleteNum);// delete the number of record
    int InsertRecord(string tableName, int recordNum);  // increment the number of record
    int GetRecord(string tableName);
    int GetIndexList(string tableName, vector<string>* indexNameVector);
    int GetAllIndex(vector<IndexInfo> * indexs);
    int SetIndex(string tableName,string AttributeName,string indexName);
    int AddTable(string tableName, vector<Attribute>* attributeVector, string primaryKeyName ,int primaryKeyLocation );
    int GetIndexType(string indexName);
    int GetAtrribute(string tableName, vector<Attribute>* attributeVector);
    int GetLength(string tableName);
    int GetLengthint(int type);
    void GetRecordString(string tableName, vector<string>* recordContent, char* recordResult);
};




#endif