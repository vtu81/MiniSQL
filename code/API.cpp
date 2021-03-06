#include "API.h"
#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"


/*Written by 蒋雨舟 START*/
int API::showRecord(string table_name, vector<string>* attribute_names) {
	int count = 0;
	if (!cm->IsTable(table_name)) {
		cout << "Table doesn't exist!" << endl;
		return -1;
	}
	vector<Condition> conditions;
	int NULL_flag = 0;
	if (attribute_names == NULL) {
		NULL_flag = 1;
		attribute_names = new vector<string>();
		Attribute temp = cm->GetAttribute(table_name);
		int i;
		for (i = 0; i < temp.num; i++) {
			attribute_names->push_back(temp.name[i]);
		}
	}
	for (int j = 0; j < attribute_names->size(); j++) {
		cout << (*attribute_names)[j] << "\t";
	}
	cout << endl;
	count += rm->recordAllShow(table_name, attribute_names, &conditions);
	if (NULL_flag) delete attribute_names;
	return count;
}

int API::showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions) {
	int count = 0;
	if (!cm->IsTable(table_name)) {
		cout << "Table doesn't exist!" << endl;
		return -1;
	}
	int NULL_flag = 0;
	if (attribute_names == NULL) {
		NULL_flag = 1;
		attribute_names = new vector<string>();
		Attribute temp = cm->GetAttribute(table_name);
		int i;
		for (i = 0; i < temp.num; i++) {
			attribute_names->push_back(temp.name[i].c_str());
		}
	}
	for (vector<string>::iterator it = attribute_names->begin(); it != attribute_names->end(); ++it) {
		cout << *it << "\t";
	}
	cout << endl;
	Attribute table_attr = cm->GetAttribute(table_name);
	int blockID=-1;
	int m, n, k;
	string IndexName;
	string IndexKey;
	int type=-2;
	Index tmpIndex;
	for (m = 0; m < conditions->size(); m++) {
		if ((*conditions)[m].operate == Condition::OPERATOR_EQUAL) {
			for (n = 0; n < table_attr.num; n++) {
				if (table_attr.name[n] == (*conditions)[m].attributeName&&table_attr.isindex[n]) {
					tmpIndex = cm->GetIndex(table_name);
					for (k = 0; k < tmpIndex.num; k++) {
						if (tmpIndex.location[k] == n) {
							IndexName = tmpIndex.indexname[k];
							IndexKey = (*conditions)[m].value;
							type = table_attr.type[n];
						}
					}
					blockID = im->searchIndex(rm->getIndexFileName(table_name, IndexName), IndexKey, type);
					break;
				}
			}
		}
		if (blockID != -1) break;
	}
	if (blockID == -1) {
		//cout << "search without index" << endl;
		count += rm->recordAllShow(table_name, attribute_names, conditions);
	}
	else {
		//cout << "search with index" << endl;
		count+=rm->recordBlockShow(table_name, attribute_names, conditions,blockID);
	}
	if (NULL_flag) delete attribute_names;
	return count;
}

void API::insertRecord(string table_name, vector<string>* record_content) {
	if (!cm->IsTable(table_name)) {
		cout << "No such table" << endl;
		return;
	}
	string indexName;
	bool haveIndex = false;
	Attribute tableAttribute = cm->GetAttribute(table_name);
	vector<SingleAttribute> attributeVector;
	vector<Condition> conditionVector;
	attributeGet(table_name, &attributeVector);
	for (int i = 0; i < tableAttribute.num; i++) {
		//indexName = attributeVector[i].indexNameGet();
		if (tableAttribute.isindex[i]) {
			//cout << "Attribute with index:" << tableAttribute.name[i] << endl;
			haveIndex = true;
		}
		else if (attributeVector[i].ifUnique)
		{
			Condition condition(attributeVector[i].name, (*record_content)[i], Condition::OPERATOR_EQUAL);
			conditionVector.insert(conditionVector.end(), condition);
		}

	}

	if (conditionVector.size() > 0)
	{
		for (int i = 0; i < conditionVector.size(); i++) {
			vector<Condition> conditionTmp;
			conditionTmp.insert(conditionTmp.begin(), conditionVector[i]);

			int recordConflictNum = rm->recordAllFind(table_name, &conditionTmp);
			if (recordConflictNum > 0) {
				cout << "Insert failed because unique value exists!" << endl;
				return;
			}
		}
	}
	char recordString[2000];
	memset(recordString, 0, 2000);
	recordStringGet(table_name, record_content, recordString);
	int recordSize = recordSizeGet(table_name);
	int blockOffset = rm->recordInsert(table_name, recordString, recordSize);
	if (haveIndex)
	{
		insertRecordIndex(table_name, recordString, recordSize, tableAttribute, blockOffset);
	}
	//std::cout << "insert record into table " << table_name << " successful" << std::endl;
}

int API::deleteRecord(string table_name) {
	int count = 0;
	vector<Condition> conditionVector;
	count+=deleteRecord(table_name, &conditionVector);
	return count;
}

int API::deleteRecord(string table_name, vector<Condition>* conditions) {
	int count=0;
	if (!cm->IsTable(table_name)) {
		cout << "No such table" << endl;
		return -1;
	}
	vector<SingleAttribute> attributeVector;
	attributeGet(table_name, &attributeVector);
	Attribute attr_info;
	attr_info = cm->GetAttribute(table_name);

	int blockID;
	int recordSize = recordSizeGet(table_name);
	count+=rm->recordAllDelete(table_name, conditions);
	return count;
}

int API::attributeGet(string tableName, vector<SingleAttribute> *attributeVector) {
	if (!cm->IsTable(tableName)) {
		return 0;
	}
	Attribute Attr_info = cm->GetAttribute(tableName);
	int i;
	int j;
	int type;
	Index attrindex;
	for (i = 0; i < Attr_info.num; i++) {
		SingleAttribute tmp = SingleAttribute(Attr_info.name[i], Attr_info.type[i], !Attr_info.repeat[i]);
		if (Attr_info.isindex[i]) {
			attrindex = cm->GetIndex(tableName);
			for (j = 0; j < 10; j++) {
				if (attrindex.location[j] == i) {
					tmp.index = attrindex.indexname[j];
					break;
				}
			}
		}
		attributeVector->push_back(tmp);
	}
	return i;
}

void API::recordStringGet(string tableName, vector<string>* recordContent, char* recordResult) {
	Attribute tableAttributes;
	tableAttributes = cm->GetAttribute(tableName);
	char* contentBegin = recordResult;
	for (int i = 0; i < tableAttributes.num; i++) {
		string content = (*recordContent)[i];
		int type = tableAttributes.type[i];
		int typeSize;
		if (type == -1) typeSize = sizeof(float);
		else if (type == 0) typeSize = sizeof(int);
		else typeSize = sizeof(char)*type;
		stringstream ss;
		ss << content;
		if (type == -1)
		{
			int intTmp;
			ss >> intTmp;
			memcpy(contentBegin, ((char*)&intTmp), typeSize);
		}
		else if (type == 0)
		{
			float floatTmp;
			ss >> floatTmp;
			memcpy(contentBegin, ((char*)&floatTmp), typeSize);
		}
		else
		{
			memset(contentBegin, 0, typeSize);
			memcpy(contentBegin, content.c_str(), content.length());
		}
		contentBegin += typeSize;
	}
}

int API::recordSizeGet(string tableName) {
	if (!cm->IsTable(tableName)) {
		cout << "Table not exist!" << endl;
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
	return recordSize;
}
/*Written by 蒋雨舟 END*/

/*Written by 谢廷浩 START*/
//创建索引
void API::createIndex(string index_name, string table_name, string attribute_name)
{
	try
	{
		//初始化catalog数据，并检测异常
		cm->CreateIndex(table_name, attribute_name, index_name);
	}
	catch (table_not_exist e)
	{
		cout << "Table " << table_name << " not exists!" << endl;
		return;
	}
	catch (attribute_not_exist e)
	{
		cout << "Attribute " << attribute_name << " not exists!" << endl;
		return;
	}
	catch (index_full e)
	{
		cout << "Indices on table " << table_name << " full!" << endl;
		return;
	}
	catch (index_exist e)
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
		if (attribute_info.name[i] == attribute_name)
		{
			//找到的type
			type = attribute_info.type[i];
			break;
		}
		if (i == attribute_info.num)
		{
			cout << "[debug]Something went wrong in API::createIndex()!" << endl;
		}
	}

	//初始化index文件
	if (rm->createIndex(table_name, index_name) == -1)
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
	//获得index对应的attribute名字
	string attribute_name;
	try
	{
		attribute_name = cm->IndextoAttr(table_name, index_name);
	}
	catch (index_not_exist e)
	{
		cout << "Index " << index_name << " on table " << table_name << " not exists!" << endl;
		return;
	}
	catch (table_not_exist e)
	{
		cout << "Table " << table_name << " not exists!" << endl;
		return;
	}

	//在catalog manager中删除index相关信息
	try
	{
		cm->DropIndex(table_name, index_name);
	}
	catch (table_not_exist e)
	{
		cout << "Table " << table_name << " not exists!" << endl;
		return;
	}
	catch (index_not_exist e)
	{
		cout << "Index " << index_name << " on table " << table_name << " not exists!" << endl;
		return;
	}

	//至此应该不需要再检测异常了

	//保存cm中获取的attribute信息
	Attribute attribute_info = cm->GetAttribute(table_name);

	//保存该index对应的type
	int type = 0;
	for (int i = 0; i < attribute_info.num; i++)
	{
		if (attribute_info.name[i] == attribute_name)
		{
			//找到的type
			type = attribute_info.type[i];
			break;
		}
		if (i == attribute_info.num)
		{
			cout << "[debug]Something went wrong in API::dropIndex()!" << endl;
		}
	}

	//将index文件删除
	rm->dropIndex(table_name, index_name);
	//删除内存中的索引文件
	im->dropIndex(rm->getIndexFileName(table_name, index_name), type);
	cout << "Drop index " << index_name << " on table " << table_name << " successfully!" << endl;
}
//内部函数，插入**单个数据**对应的索引
void API::insertIndex(string table_name, string index_name, char* key_addr, int type, int blockID)
{
	string key_str;
	stringstream ss;
	//将内存中对应位置的数据转换为字符串，保存到key_str中
	if (type == IndexManager::TYPE_FLOAT)
	{
		float value = *(float*)key_addr;
		ss << value;
		ss >> key_str;
	}
	else if (type == IndexManager::TYPE_INT)
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
	if (type == IndexManager::TYPE_FLOAT)
	{
		float value = *(float*)key_addr;
		ss << value;
		ss >> key_str;
	}
	else if (type == IndexManager::TYPE_INT)
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
	for (int i = 0; i < indices_info.num; i++)
	{
		attribute_index_id[indices_info.location[i]] = i;
	}
	//遍历该条记录中的每一个data
	for (int i = 0; i < attributes.num; i++)
	{
		//获取当前attribute的种类type
		int type = attributes.type[i];
		//获取当前attribute的大小size
		int size;
		if (type == IndexManager::TYPE_FLOAT) size = sizeof(float);
		else if (type == IndexManager::TYPE_INT) size = sizeof(int);
		else size = type;
		//如果该条记录有索引
		if (attribute_index_id[i] >= 0)
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
	for (int i = 0; i < indices_info.num; i++)
	{
		attribute_index_id[indices_info.location[i]] = i;
	}
	//遍历该条记录中的每一个data
	for (int i = 0; i < attributes.num; i++)
	{
		//获取当前attribute的种类type
		int type = attributes.type[i];
		//获取当前attribute的大小size
		int size;
		if (type == IndexManager::TYPE_FLOAT) size = sizeof(float);
		else if (type == IndexManager::TYPE_INT) size = sizeof(int);
		else size = type;
		//如果该条记录有索引
		if (attribute_index_id[i] >= 0)
		{
			//调用删除索引API实现插入索引
			insertIndex(table_name, indices_info.indexname[attribute_index_id[i]], tmp, type, blockID);
		}
		//移至下一条记录首地址
		tmp += size;
	}
}
//Index Manager用到的内部函数，获取所有index的信息：<index文件名，index数据类型>
vector<pair<string, int> > API::allIndexInfoGet()
{
	vector<pair<string, int> > all_index_info;
	vector<string> all_table_list;
	//获取数据库中每一张表的表名
	all_table_list = cm->GetAllTable();
	//遍历数据库中每一张表
	for (auto &table_name : all_table_list)
	{
		//获取当前遍历到的表（名）对应的表信息
		Attribute attr = cm->GetAttribute(table_name);
		//获取当前遍历到的表（名）对应的索引信息
		Index indices_info = cm->GetIndex(table_name);
		vector<int> attribute_index_id(attr.num, -1); //用于表示对应的attribute索引在indices_info中的编号；先全部初始化为-1
											//-1表示没有索引；否则表示索引编号
		//标记对应的attribute索引编号
		for (int i = 0; i < indices_info.num; i++)
		{
			attribute_index_id[indices_info.location[i]] = i;
		}
		//遍历该表中的每一个属性
		for (int i = 0; i < attr.num; i++)
		{
			//获取当前attribute的种类type
			int type = attr.type[i];
			//如果该属性有索引
			if (attribute_index_id[i] >= 0)
			{
				//获取索引文件名
				string file_name = rm->getIndexFileName(table_name, indices_info.indexname[i]);
				//将该索引信息记录至all_index_info中
				all_index_info.push_back(pair<string, int>(file_name, type));
			}
		}
	}
	return all_index_info;
}
/*Written by 谢廷浩 END*/


/*start by 俞晓锋*/
bool API::dropTable(std::string t_name)
{
	try
	{
        //先删除表的所有索引
        Attribute attributes = cm->GetAttribute(t_name); //保存cm中获取的attribute信息
        vector<int> attribute_index_id(attributes.num, -1); //用于表示对应的attribute索引在indices_info中的编号；先全部初始化为-1
                                                            //-1表示没有索引；否则表示索引编号
        //获取该表的索引信息
        Index indices_info = cm->GetIndex(t_name);
        //标记对应的attribute索引编号
        for (int i = 0; i < indices_info.num; i++)
        {
            attribute_index_id[indices_info.location[i]] = i;
        }
        //遍历该表中的每一个属性
        for (int i = 0; i < attributes.num; i++)
        {
            //当前属性的type
            int type = attributes.type[i];
            //如果该属性有索引
            if (attribute_index_id[i] >= 0)
            {
                //当前属性对应索引名
                string index_name = indices_info.indexname[attribute_index_id[i]];
                //将index文件删除
                rm->dropIndex(t_name, index_name);
                //删除内存中的索引文件
                im->dropIndex(rm->getIndexFileName(t_name, index_name), type);
            }
        }
		rm->dropTable(t_name);
		cm->DropTable(t_name);
	}
	catch (table_not_exist e)
	{
		cout << "Table " << t_name << " not exists!" << endl;
		return false;
	}

	cout << "Drop table " << t_name << " successfully!" << endl;
	return true;
}
bool API::createTable(std::string t_name, Attribute attribute, int primary, Index index)
{
	try
	{
		cm->CreateTable(t_name, attribute, primary, index);
		rm->createTable(t_name);
	}
	catch (table_exist e)
	{
		cout << "Table " << t_name << " already exists!" << endl;
		return false;
	}

	/***********************************对主键建立索引开始**************************************/
	//createIndex("PRIMARY_KEY_" + t_name, t_name, attribute.name[primary]);
	try
	{
		//初始化catalog数据，并检测异常
		cm->CreateIndex(t_name, attribute.name[primary], "PRIMARY_KEY");
	}
	catch (table_not_exist e)
	{
		cout << "[debug]In API::createTable(); Table " << t_name << " not exists!" << endl;
		return false;
	}
	catch (attribute_not_exist e)
	{
		cout << "[debug]In API::createTable(); Attribute " << t_name << " not exists!" << endl;
		return false;
	}
	catch (index_full e)
	{
		cout << "[debug]In API::createTable(); Indices on table " << t_name << " full!" << endl;
		return false;
	}
	catch (index_exist e)
	{
		cout << "[debug]In API::createTable(); Index " << "PRIMARY_KEY" << " already exists!" << endl;
		return false;
	}

	//至此应该不需要再检测异常了

	//保存cm中获取的attribute信息
	Attribute attribute_info = cm->GetAttribute(t_name);
	//保存该index对应的type
	int type = 0;
	for (int i = 0; i < attribute_info.num; i++)
	{
		if (attribute_info.name[i] == attribute.name[primary])
		{
			//找到的type
			type = attribute_info.type[i];
			break;
		}
		if (i == attribute_info.num)
		{
			cout << "[debug]In API::createTable(); Something went wrong!" << endl;
		}
	}

	//初始化index文件
	if (rm->createIndex(t_name, "PRIMARY_KEY") == -1)
	{
		cout << "[debug]In API::createTable(); Cannot create index file on disk. Make sure file path ./database/index/ exists!" << endl;
		return false;
	}

	//通过im创建真实的B+树index文件
	im->createIndex(rm->getIndexFileName(t_name, "PRIMARY_KEY"), type);
	//将已有的record插入index文件中
	rm->indexRecordAllAlreadyInsert(t_name, "PRIMARY_KEY");
	/***********************************对主键建立索引结束**************************************/

	cout << "Create table " << t_name << " successfully!" << endl;
	return true;
}
/*end by 俞晓锋*/
