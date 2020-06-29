#include "Index_Manager.h"
#include "API.h"


IndexManager::IndexManager(API* api)
{
    api_ = api;
}

void IndexManager::init()
{
    //保存所有index的信息：<index文件名，index的type>
    std::vector<pair<string, int>> all_index_info;
    //获取所有index的信息
    all_index_info = api_->allIndexInfoGet();
    //这些索引之前已经创建，因此在磁盘上找到对应的文件进行读取、重构B+树
    //cout << "[debug] In IndexManager::init(); All indices created before: " << endl;
    for(auto &i: all_index_info)
    {
        //std::cout << "@" << i.first << endl;
        createIndex(i.first, i.second);
    }
}

IndexManager::~IndexManager()
{
    //将所有map中保存的B+树记录到磁盘 -- delete暂时有问题，先注释掉，不影响代码正常运行
    for(auto& i: float_index_map_)
    {
        if(i.second)
        {
            i.second->write_back_to_disk_all();
            delete i.second;
        }
    }
    for(auto& i: int_index_map_)
    {
        if(i.second)
        {
            i.second->write_back_to_disk_all();
            delete i.second;
        }
    }
    for(auto& i: string_index_map_)
    {
        if(i.second)
        {
            i.second->write_back_to_disk_all();
            delete i.second;
        }
    }
}

void IndexManager::createIndex(std::string file_name, int type)
{
    //获取键的大小
    int key_size;
    if(type == TYPE_FLOAT)
        key_size = sizeof(float);
    else if(type == TYPE_INT)
        key_size = sizeof(int);
    else if(type > 0)
        key_size = type;
    else
    {
        std::cout << "[debug]Error: In IndexManager::createIndex(); Invalid type!"<<std::endl; //调试用
        return;
    }
    
    //获取B+树的合适度数；和B+树记录到磁盘上的数据格式有关，可能需要调整
    //预留了一个位置给单个block中的结束符'#'
    //To be continued.
    int degree;
    degree = (PAGESIZE - 1) / (key_size + sizeof(int));
    if(degree % 2 == 1) degree--;
    //根据类型新建一棵B+树，放入相应的map中
    if(type == TYPE_FLOAT)
    {
        BpTree<float, int> *tree = new BpTree<float, int>(file_name, key_size, degree);
        float_index_map_.insert(FLOATMAP::value_type(file_name, tree));
    }
    else if(type == TYPE_INT)
    {
        BpTree<int, int> *tree = new BpTree<int, int>(file_name, key_size, degree);
        int_index_map_.insert(INTMAP::value_type(file_name, tree));
    }
    else
    {
        BpTree<std::string, int> *tree = new BpTree<std::string, int>(file_name, key_size, degree);
        string_index_map_.insert(STRINGMAP::value_type(file_name, tree));
    }
}

void IndexManager::dropIndex(std::string file_name, int type)
{
    if(type == TYPE_FLOAT)
    {
        FLOATMAP::iterator i = float_index_map_.find(file_name);
        if(i == float_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::dropIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        //释放B+树内存
        delete i->second;
        float_index_map_.erase(i);
    }
    else if(type == TYPE_INT)
    {
        INTMAP::iterator i = int_index_map_.find(file_name);
        if(i == int_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::dropIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        //释放B+树内存
        delete i->second;
        int_index_map_.erase(i);
    }
    else
    {
        STRINGMAP::iterator i = string_index_map_.find(file_name);
        if(i == string_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::dropIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        //释放B+树内存
        delete i->second;
        string_index_map_.erase(i);
    }
    //注意：不会自动删除磁盘上的索引文件，需要在API层进一步删除！
}

int IndexManager::searchIndex(std::string file_name, std::string key, int type)
{
    //转换字符串key为对应类型的真实key
    float key_float;
    int key_int;
    std::string key_string;
    std::stringstream ss(key);
    if(type == TYPE_FLOAT)
    {
        ss >> key_float;
    }
    else if(type == TYPE_INT)
    {
        ss >> key_int;
    }
    else
    {
        ss >> key_string;
    }

    //找到对应的B+树，执行单键搜索操作
    //找到的话返回记录所在blockID
    //找不到key则返回-1
    //没有该index返回-2
    if(type == TYPE_FLOAT)
    {
        FLOATMAP::iterator i = float_index_map_.find(file_name);
        if(i == float_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::searchIndex(); No index " << file_name << " exists!" << std::endl;
            return -2;
        }
        int* ret_float_ptr;
        if((ret_float_ptr = i->second->search(key_float)) != nullptr)
        {
            return *ret_float_ptr; 
        }
        //std::cout << "[debug]Error: In IndexManager::searchIndex(); No key with value " << key_float << " exists in index " << file_name << std::endl;
        return -1;
    }
    else if(type == TYPE_INT)
    {
        INTMAP::iterator i = int_index_map_.find(file_name);
        if(i == int_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::searchIndex(); No index " << file_name << " exists!" << std::endl;
            return -2;
        }
        int* ret_int_ptr;
        if((ret_int_ptr = i->second->search(key_int)) != nullptr)
        {
            return *ret_int_ptr;
        }
        //std::cout << "[debug]Error: In IndexManager::searchIndex();  No key with value " << key_int << " exists in index " << file_name << std::endl;
        return -1;
    }
    else
    {
        STRINGMAP::iterator i = string_index_map_.find(file_name);
        if(i == string_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::searchIndex(); No index " << file_name << " exists!" << std::endl;
            return -2;
        }
        int* ret_string_ptr;
        if((ret_string_ptr = i->second->search(key_string)) != nullptr)
        {
            return *ret_string_ptr;
        }
        //std::cout << "[debug]Error: In IndexManager::searchIndex(); No key with value " << key_string << " exists in index " << file_name << std::endl;
        return -1; 
    }
}

void IndexManager::insertIndex(std::string file_name, std::string key, int blockID, int type)
{
    //转换字符串key为对应类型的真实key
    float key_float;
    int key_int;
    std::string key_string;
    std::stringstream ss(key);
    if(type == TYPE_FLOAT)
    {
        ss >> key_float;
    }
    else if(type == TYPE_INT)
    {
        ss >> key_int;
    }
    else
    {
        ss >> key_string;
    }

    //找到对应的B+树，执行单键插入操作
    if(type == TYPE_FLOAT)
    {
        FLOATMAP::iterator i = float_index_map_.find(file_name);
        if(i == float_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::insertIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->insert(key_float, blockID))
            std::cout << "[debug]Error: In IndexManager::insertIndex(); Index insertion failed because of duplicate key!" << std::endl;
    }
    else if(type == TYPE_INT)
    {
        INTMAP::iterator i = int_index_map_.find(file_name);
        if(i == int_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::insertIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->insert(key_int, blockID))
            std::cout << "[debug]Error: In IndexManager::insertIndex(); Index insertion failed because of duplicate key!" << std::endl;
    }
    else
    {
        STRINGMAP::iterator i = string_index_map_.find(file_name);
        if(i == string_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::insertIndex(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->insert(key_string, blockID))
            std::cout << "[debug]Error: In IndexManager::insertIndex(); Index insertion failed because of duplicate key!" << std::endl;
    }
}

void IndexManager::deleteIndexByKey(std::string file_name, std::string key, int type)
{
    //转换字符串key为对应类型的真实key
    float key_float;
    int key_int;
    std::string key_string;
    std::stringstream ss(key);
    if(type == TYPE_FLOAT)
    {
        ss >> key_float;
    }
    else if(type == TYPE_INT)
    {
        ss >> key_int;
    }
    else
    {
        ss >> key_string;
    }


    //找到对应的B+树，执行单键删除操作
    if(type == TYPE_FLOAT)
    {
        FLOATMAP::iterator i = float_index_map_.find(file_name);
        if(i == float_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); Error: No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->delete_single(key_float))
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); Index deletion failed because of duplicate key!" << std::endl;
    }
    else if(type == TYPE_INT)
    {
        INTMAP::iterator i = int_index_map_.find(file_name);
        if(i == int_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); Error: No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->delete_single(key_int))
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); Index deletion failed because of duplicate key!" << std::endl;
    }
    else
    {
        STRINGMAP::iterator i = string_index_map_.find(file_name);
        if(i == string_index_map_.end())
        {
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); No index " << file_name << " exists!" << std::endl;
            return;
        }
        if(!i->second->delete_single(key_string))
            std::cout << "[debug]Error: In IndexManager::deleteIndexByKey(); Index deletion failed because of duplicate key!" << std::endl;
    }
}