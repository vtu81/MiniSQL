#pragma once
#ifndef _API_H_
#define _API_H_

#include <iostream>
#include <vector>
#include <string>
#include "Condition.h"
// #include "SingleAttribute.h" //原本是Attribute.h，现在的Attribute在Basic.h中
#include "Basic.h"
/*目录下暂时没有的头文件，以后可能会用到*/
// #include "IndexInfo.h"
// #include "Exception.h"

using namespace std;

class CatalogManager;
class RecordManager;
class IndexManager;

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
    API(){};
    ~API(){};
    //创建表
    bool createTable(std::string table_name, Attribute attribute, int primary, Index index);
    //删除表
    bool dropTable(std::string table_name);
    //创建索引
    void createIndex(string index_name, string table_name, string attribute_name);
    //删除索引
    void dropIndex(string table_name, string index_name);
    //展示记录 -- 无条件
    void showRecord(string table_name, vector<string>* attribute_names);
    //展示记录 -- 有条件
    void showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions);
    //插入记录
    void insertRecord(string table_name, vector<string>* record_content);
    //删除记录 -- 无条件
    void deleteRecord(string table_name);
    //删除记录 -- 有条件
    void deleteRecord(string table_name, vector<Condition>* conditions);
    
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
    vector<pair<string, int>> allIndexInfoGet();
    //To be continued.
};

#endif
