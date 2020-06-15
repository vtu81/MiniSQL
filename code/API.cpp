#include "API.h"
#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"




/*Written by 谢廷浩 START*/
void API::createIndex(string index_name, string table_name, string attribute_name)
{
    try
    {
        //初始化catalog数据，并检测异常
        cm->CreateIndex(index_name, table_name, attribute_name);
    }
    catch(table_not_exist)
    {
        cout << "Table " << table_name << " not exists!" << endl;
        return;
    }
    catch(attribute_not_exist)
    {
        cout << "Attribute " << attribute_name << " not exists!" << endl;
        return;
    }
    catch(index_full)
    {
        cout << "Indices on table " << table_name << " full!" << endl;
        return;
    }
    catch(index_exist)
    {
        cout << "Index " << index_name << " already exists!" << endl;
        return;
    }

    //至此应该不需要再检测异常了

    //保存cm中获取的attribute信息
    Attribute attribute_info = cm->GetAttribute(table_name);
    //保存该index对应的type
    int type = 0;
    for (int i = 0; i < attribute_info.num; i++)
    {
        if(attribute_info.name[i] == attribute_name)
        {
            //找到的type
            type = attribute_info.type[i];
            break;
        }
        if(i == attribute_info.num)
        {
            cout << "[debug]Something went wrong in API::createIndex()!" << endl;
        }
    }

    //初始化index文件
    if(rm->createIndex(index_name) == -1)
    {
        cout << "Cannot create index file on disk. Make sure file path ./database/index/ exists!" << endl;
        return;
    }

    //通过im创建真实的B+树index文件
    im->createIndex(rm->getIndexFileName(index_name), type);
    //将已有的record插入index文件中
    rm->indexRecordAllAlreadyInsert(table_name, index_name);
    cout << "Create index " << index_name << " on table " << table_name << " successfully!" << endl;
}

void API::dropIndex(string table_name, string index_name)
{
    try
    {
        cm->DropIndex(table_name, index_name);
    }
    catch(table_not_exist)
    {
        cout << "Table " << table_name << " not exists!" << endl;
        return;
    }
    catch(index_not_exist)
    {
        cout << "Index " << index_name << " on table " << table_name << " not exists!" << endl;
        return;
    }
    
    //至此应该不需要再检测异常了

    //保存cm中获取的attribute信息
    Attribute attribute_info = cm->GetAttribute(table_name);
    //获得index对应的attribute名字
    string attribute_name = cm->IndextoAttr(table_name, index_name);
    //保存该index对应的type
    int type = 0;
    for (int i = 0; i < attribute_info.num; i++)
    {
        if(attribute_info.name[i] == attribute_name)
        {
            //找到的type
            type = attribute_info.type[i];
            break;
        }
        if(i == attribute_info.num)
        {
            cout << "[debug]Something went wrong in API::dropIndex()!" << endl;
        }
    }

    //将index文件删除
    rm->dropIndex(index_name);
    //
    im->dropIndex(rm->getIndexFileName(index_name), type);
    cout << "Drop index " << index_name << " on table " << table_name << " successfully!" << endl;
}

//以下3个内部函数还没有开始写
//To be continued.
void insertIndex(string index_name, char* key_addr, int type, int blockID)
{

}
void deleteRecordIndex(char* record_begin, int record_size, Attribute attributes, int blockID)
{

}
void insertRecordIndex(char* record_begin, int record_size, Attribute attributes, int blockID)
{
    
}
/*Written by 谢廷浩 END*/