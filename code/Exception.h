//
// Created by 邱泽鸿 on 2020/6/13.
//

#ifndef BPLUSTREE_EXCEPTION_H
#define BPLUSTREE_EXCEPTION_H
#include <iostream>
#include <string>


typedef enum // 异常类型
{
    EXISTED_KEY = 0, // 插入数据时，数据的key值已经存在
    DELETING_KEY_NOT_FOUND, // 删除数据时，对应的Key值不存在

} Type;

class Exception
{
public:
    Type type;
    std::string info; // 异常信息

public:
    Exception(Type type)
    : type(type), info("")
    {

    }
    Exception(Type& type, std::string& info)
    : type(type), info(info)
    {

    }
};

#endif //BPLUSTREE_EXCEPTION_H
