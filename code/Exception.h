//
// Created by 邱泽鸿 on 2020/6/13.
//

#ifndef BPLUSTREE_EXCEPTION_H
#define BPLUSTREE_EXCEPTION_H
#include <iostream>
#include <string>


typedef enum // 异常类型
{
    //应该在API模块中提前判定以避免异常！但以防万一，还是在必要时抛出异常
    INDEX_KEY_EXISTED = 0, // 插入数据时，数据的key值已经存在
    INDEX_DELETE_KEY_NOT_FOUND, // 删除数据时，对应的Key值不存在

} Type;
//Exception异常应当尽可能被避免，如果发生了则是预期之外的情形，表示代码有问题
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
//下面的异常为预期内的异常！主要在catalog_manager中抛出，在API中处理
class table_exist{};
class table_not_exist{};
class attribute_not_exist{};
class index_not_exist{};
class index_full{};
class index_exist{};

// interpreter出现的异常
class multiple_tables{}; // 试图访问多个表
class invalid_operator{}; // 比较操作符（大于、等于、小于、...）错误
class insert_syntax_error{}; // 插入语句语法错误
class delete_syntax_error{}; // 删除语句语法错误
class create_table_syntax_error{}; // 创建表格语句语法错误
class create_index_syntax_error{}; // 创建索引语法错误
class char_size_overflow{}; // char类型数组的长度为0~255
class unknown_data_type{}; // 创建表格指定的类型错误
class primary_key_not_exist{}; // 创建表格时没有指明主码

#endif //BPLUSTREE_EXCEPTION_H
