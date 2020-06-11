#include "Index_Manager.h"


IndexManager::IndexManager(API* api)
{
    api_ = api;
    //To be continued.
}

IndexManager::~IndexManager()
{

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

}

int IndexManager::searchIndex(std::string file_name, std::string key, int type)
{

}

void IndexManager::insertIndex(std::string file_name, int blockID, int type)
{

}

void IndexManager::deleteIndexByKey(std::string file_name, std::string key, int type)
{
    
}