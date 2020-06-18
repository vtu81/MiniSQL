#include "API.h"
#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"


/*Written by 蒋雨舟 START*/
void API::showRecord(string table_name, vector<string>* attribute_names) {
	if (!cm->IsTable(table_name)) {
		cout << "Table doesn't exist!" << endl;
		return;
	}
	vector<Condition> conditions;
	if (attribute_names == NULL) {
		Attribute temp = cm->GetAttribute(table_name);
		int i;
		for (i = 0; i < temp.num; i++) {
			attribute_names->push_back(temp.name[i].c_str());
		}
	}
	for (int j = 0; j < attribute_names->size(); j++) {
		printf("%s", attribute_names[j]);
		printf(" ");
	}
	printf("\n");
	rm->recordAllShow(table_name, attribute_names, &conditions);
}

void API::showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions) {
	if (!cm->IsTable(table_name)) {
		cout << "Table doesn't exist!" << endl;
		return;
	}
	if (attribute_names == NULL) {
		Attribute temp = cm->GetAttribute(table_name);
		int i;
		for (i = 0; i < temp.num; i++) {
			attribute_names->push_back(temp.name[i].c_str());
		}
	}
	for (int j = 0; j < attribute_names->size(); j++) {
		printf("%s", attribute_names[j]);
		printf(" ");
	}
	printf("\n");
	rm->recordAllShow(table_name, attribute_names, conditions);
}
int API::attributeGet(string tableName, vector<SingleAttribute> *attributeVector) {
	if (!cm->IsTable(tableName)) {
		return 0;
	}
	Attribute Attr_info = cm->GetAttribute(tableName);
	int i;
	for (i = 0; i < Attr_info.num; i++) {
		SingleAttribute tmp = SingleAttribute(Attr_info.name[i], Attr_info.type[i], Attr_info.repeat[i]);
		attributeVector->push_back(tmp);
	}
	return i;
}

int API::recordSizeGet(string tableName) {
	if (!cm->IsTable) {
		cout << "Table not exist" << endl;
		return -1;
	}
	Attribute attr_info = cm->GetAttribute(tableName);
	int recordSize = 0;
	int i;
	for (i = 0; i < attr_info.num; i++) {
		if (attr_info.type[i] == -1) {
			recordSize += sizeof(float);
		}
		else if (attr_info.type[i] == 0) {
			recordSize += sizeof(int);
		}
		else {
			recordSize += sizeof(char)*attr_info.type[i];
		}
	}
}
/*Written by 蒋雨舟 END*/

/*Written by 谢廷浩 START*/
//创建索引
void API::createIndex(string index_name, string table_name, string attribute_name)
{
    try
    {
        //初始化catalog数据，并检测异常
        cm->CreateIndex(index_name, table_name, attribute_name);
    }
    catch(table_not_exist e)
    {
        cout << "Table " << table_name << " not exists!" << endl;
        return;
    }
    catch(attribute_not_exist e)
    {
        cout << "Attribute " << attribute_name << " not exists!" << endl;
        return;
    }
    catch(index_full e)
    {
        cout << "Indices on table " << table_name << " full!" << endl;
        return;
    }
    catch(index_exist e)
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
    im->createIndex(rm->getIndexFileName(table_name, index_name), type);
    //将已有的record插入index文件中
    rm->indexRecordAllAlreadyInsert(table_name, index_name);
    cout << "Create index " << index_name << " on table " << table_name << " successfully!" << endl;
}
//删除某个索引
void API::dropIndex(string table_name, string index_name)
{
    try
    {
        cm->DropIndex(table_name, index_name);
    }
    catch(table_not_exist e)
    {
        cout << "Table " << table_name << " not exists!" << endl;
        return;
    }
    catch(index_not_exist e)
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
    im->dropIndex(rm->getIndexFileName(table_name, index_name), type);
    cout << "Drop index " << index_name << " on table " << table_name << " successfully!" << endl;
}
//内部函数，插入**单个数据**对应的索引
void API::insertIndex(string table_name, string index_name, char* key_addr, int type, int blockID)
{
    string key_str;
    stringstream ss;
    //将内存中对应位置的数据转换为字符串，保存到key_str中
    if(type == IndexManager::TYPE_FLOAT)
    {
        float value = *(float*)key_addr;
        ss << value;
        ss >> key_str;
    }
    else if(type == IndexManager::TYPE_INT)
    {
        int value = *(int*)key_addr;
        ss << value;
        ss >> key_str;
    }
    else
    {
        key_str = std::string(key_addr, type); //type这里刚好是data size
        
        //此处可能会有问题？需要保证内存中key_addr开始的type个字节用于保存字符串，且多余的字节均为'\0'
        //（如type=10时，"Hello"对应：H e l l o \0 \0 \0 \0 \0）
        //To be continued.
        key_str.erase(std::remove(key_str.begin(), key_str.end(), '\0'), key_str.end()); //删除所有'\0'
    }
    //传递key_str等给index manager处理插入
    im->insertIndex(rm->getIndexFileName(table_name, index_name), key_str, blockID, type);
}
//内部函数，删除**单个数据**对应的索引
void API::deleteIndex(string table_name, string index_name, char* key_addr, int type)
{
    string key_str;
    stringstream ss;
    //将内存中对应位置的数据转换为字符串，保存到key_str中
    if(type == IndexManager::TYPE_FLOAT)
    {
        float value = *(float*)key_addr;
        ss << value;
        ss >> key_str;
    }
    else if(type == IndexManager::TYPE_INT)
    {
        int value = *(int*)key_addr;
        ss << value;
        ss >> key_str;
    }
    else
    {
        key_str = std::string(key_addr, type); //type这里刚好是data size
        
        //此处可能会有问题？需要保证内存中key_addr开始的type个字节用于保存字符串，且多余的字节均为'\0'
        //（如type=10时，"Hello"对应：H e l l o \0 \0 \0 \0 \0）
        //To be continued.
        key_str.erase(std::remove(key_str.begin(), key_str.end(), '\0'), key_str.end()); //删除所有'\0'
    }
    //传递key_str等给index manager处理删除
    im->deleteIndexByKey(rm->getIndexFileName(table_name, index_name), key_str, type);
}
//内部函数，删除给定地址开始的**一条记录**对应的索引
void API::deleteRecordIndex(string table_name, char* record_begin, int record_size, Attribute attributes)
{
    char* tmp = record_begin;
    vector<int> attribute_index_id(attributes.num, -1); //用于表示对应的attribute索引在indices_info中的编号；先全部初始化为-1
                                                        //-1表示没有索引；否则表示索引编号
    //获取该表的索引信息
    Index indices_info = cm->GetIndex(table_name);
    //标记对应的attribute索引编号
    for(int i = 0; i < indices_info.num; i++)
    {
        attribute_index_id[indices_info.location[i]] = i;
    }
    //遍历该条记录中的每一个data
    for(int i = 0; i < attributes.num; i++)
    {
        //获取当前attribute的种类type
        int type = attributes.type[i];
        //获取当前attribute的大小size
        int size;
        if(type == IndexManager::TYPE_FLOAT) size = sizeof(float);
        else if(type == IndexManager::TYPE_INT) size = sizeof(int);
        else size = type;
        //如果该条记录有索引
        if(attribute_index_id[i] >= 0)
        {
            //调用删除索引API实现插入索引
            deleteIndex(table_name, indices_info.indexname[attribute_index_id[i]], tmp, type);
        }
        //移至下一条记录首地址
        tmp += size;
    }
}
//内部函数，插入给定地址开始的**一条记录**对应的索引
void API::insertRecordIndex(string table_name, char* record_begin, int record_size, Attribute attributes, int blockID)
{
    char* tmp = record_begin;
    vector<int> attribute_index_id(attributes.num, -1); //用于表示对应的attribute索引在indices_info中的编号；先全部初始化为-1
                                                        //-1表示没有索引；否则表示索引编号
    //获取该表的索引信息
    Index indices_info = cm->GetIndex(table_name);
    //标记对应的attribute索引编号
    for(int i = 0; i < indices_info.num; i++)
    {
        attribute_index_id[indices_info.location[i]] = i;
    }
    //遍历该条记录中的每一个data
    for(int i = 0; i < attributes.num; i++)
    {
        //获取当前attribute的种类type
        int type = attributes.type[i];
        //获取当前attribute的大小size
        int size;
        if(type == IndexManager::TYPE_FLOAT) size = sizeof(float);
        else if(type == IndexManager::TYPE_INT) size = sizeof(int);
        else size = type;
        //如果该条记录有索引
        if(attribute_index_id[i] >= 0)
        {
            //调用删除索引API实现插入索引
            insertIndex(table_name, indices_info.indexname[attribute_index_id[i]], tmp, type, blockID);
        }
        //移至下一条记录首地址
        tmp += size;
    }
}
//Index Manager用到的内部函数，获取所有index的信息：<index名称，index数据类型>
vector<pair<string, int>> API::allIndexInfoGet()
{
    vector<pair<string, int>> all_index_info;
    //To be continued.
    //需要catalog_manager增加接口，获取所有表名
    return all_index_info;
}
/*Written by 谢廷浩 END*/
