#pragma once
#ifndef _API_H_
#define _API_H_

#include "Index_Manager.h"
#include "Record_Manager.h"
#include "Catalog_Manager.h"

//头文件引用and类声明还不规范；待最后一起调整
extern CatalogManager catalog_manager;
extern RecordManager record_manager;
extern IndexManager index_manager;

class API
{
private:
    /*Internal function members below*/
    /*一些内部函数，不应该被interpreter调用！
    可能会被API本身、catalog manager、record manager、index manager模块调用
    没有列全，用到时自行补足即可*/
    //To be continued.
public:
    //指向全局catalog manager的指针
    CatalogManager *cm;
    //指向全局record manager的指针
    RecordManager *rm;
    //指向全局index manager的指针
    IndexManager *im;

    /*外部接口*/
    API()
    {
        cm = &catalog_manager;
        rm = &record_manager;
        im = &index_manager;
    }
    ~API();
    //创建表
    void createTable(string table_name);
    //删除表
    void dropTable(string table_name);
    //创建索引
    void createIndex(string index_name, string table_name, string attribute_name);
    //删除索引
    void dropIndex(string table_name);
    //展示记录 -- 无条件
    void showRecord(string table_name, vector<stirng>* attribute_names);
    //展示记录 -- 有条件
    void showRecord(string table_name, vector<stirng>* attribute_names, vector<Condition>* conditions);
    //插入记录
    void insertRecord(string table_name, vector<string>* record_content);
    //删除记录 -- 无条件
    void deleteRecord(string table_name);
    //删除记录 -- 有条件
    void deleteRecord(string table_name, vector<Condition>* conditions);
    
    /*Internal function members below*/
    /*一些内部函数，不应该被interpreter调用！
    可能会被API本身、catalog manager、record manager、index manager模块调用
    没有列全，用到时自行补足即可*/
    void insertIndex(string index_name, char* key_addr, int type, int blockID);
    //To be continued.
};

#endif