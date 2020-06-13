#include "Index_Manager.h"



IndexManager::IndexManager(API* api)
{
    api_ = api;
    //To be continued.
}

IndexManager::~IndexManager()
{
    //将所有map中保存的B+树记录到磁盘
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
        std::cout << "Error: Invalid type in IndexManager::createIndex()!"<<std::endl; //调试用
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
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
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
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
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
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
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
    //否则返回-1
    if(type == TYPE_FLOAT)
    {
        FLOATMAP::iterator i = float_index_map_.find(file_name);
        if(i == float_index_map_.end())
        {
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
            return -1;
        }
        int ret_float;
        i->second->search(key_float, ret_float);
        return ret_float; 
    }
    else if(type == TYPE_INT)
    {
        INTMAP::iterator i = int_index_map_.find(file_name);
        if(i == int_index_map_.end())
        {
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
            return -1;
        }
        int ret_int;
        i->second->search(key_float, ret_int);
        return ret_int; 
    }
    else
    {
        STRINGMAP::iterator i = string_index_map_.find(file_name);
        if(i == string_index_map_.end())
        {
            std::cout << "Error: No index " << file_name << " exists!" << std::endl;
            return -1;
        }
        int ret_string;
        i->second->search(key_string, ret_string);
        return ret_string; 
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

    try
    {
        //找到对应的B+树，执行单键插入操作
        if(type == TYPE_FLOAT)
        {
            FLOATMAP::iterator i = float_index_map_.find(file_name);
            if(i == float_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->insert(key_float, blockID);
        }
        else if(type == TYPE_INT)
        {
            INTMAP::iterator i = int_index_map_.find(file_name);
            if(i == int_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->insert(key_int, blockID);
        }
        else
        {
            STRINGMAP::iterator i = string_index_map_.find(file_name);
            if(i == string_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->insert(key_string, blockID);
        }
    }
    catch (Exception& e)
    { // 插入异常

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

    try
    {
        //找到对应的B+树，执行单键删除操作
        if(type == TYPE_FLOAT)
        {
            FLOATMAP::iterator i = float_index_map_.find(file_name);
            if(i == float_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->delete_single(key_float);
        }
        else if(type == TYPE_INT)
        {
            INTMAP::iterator i = int_index_map_.find(file_name);
            if(i == int_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->delete_single(key_int);
        }
        else
        {
            STRINGMAP::iterator i = string_index_map_.find(file_name);
            if(i == string_index_map_.end())
            {
                std::cout << "Error: No index " << file_name << " exists!" << std::endl;
                return;
            }
            i->second->delete_single(key_string);
        }
    }
    catch (Exception& e)
    { // 删除异常

    }
}