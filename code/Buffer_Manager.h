#pragma once
#ifndef _BUFFER_MANAGER_H_
#define _BUFFER_MANAGER_H_

#include <iostream>
#include <cstdio>
#include <string>

#define MAXPAGEPOOLSIZE 102400
#define PAGESIZE 4096

class Page 
{
private:
	//块号
	int block_id_;
	//文件名
	std::string file_name_;
	//Page内容
	char content_[PAGESIZE];
	//pin的数量
	int pin_;
	//是否为脏块
	bool dirty_;
	//用于替换策略变量
	bool reference_;
public:
	//默认构造函数
	Page();
	//初始化函数
	void initialize();
	// get和set函数
	void setFileName(string n);
	std::string getFileName();
	
	void setBlockId(int block_id);
	int getBlockId();

	void setPin(int pin);
	int getPin();

	void setDirty(bool dirty);
	bool getDirty();

	void setRefer(bool reference);
	bool getRefer();

	char *getBuffer();
};

class BufferManager
{
public:
    //默认构造函数
    BufferManager()
    {
        initialize(MAXPAGEPOOLSIZE);
    }
    //利用page_num进行构造
    BufferManager(int page_num)
    {
        initialize(page_num);
    }
    //析构函数，output所有页
    ~BufferManager();
    //返回file_name文件的block_id块在page_pool_中的首地址
    char* fetchPage(std::string file_name, int block_id);
    //返回file_name文件的block_id块在page_pool_中page序号
    int fetchPageID(std::string file_name, int block_id);
    //标记第page_id个page为dirty
    int markPageDirty(int page_id);
    //钉住第page_id个page：pin_count++；返回改变后的pin_count
    int pinPage(int page_id);
    //解除第page_id个page的钉住状态：pin_count--；返回改变后的pin_count
    int unpinPage(int page_id);
    //将page_pool_中的第page_id个page写到磁盘；返回1表示输出成功，返回0表示输出失败
    int outputPage(int page_id);
    //int outputPage(int page_id, std::string file_name, int block_id);

private:
    //缓冲池首地址指针
    Page* page_pool_;
    //缓冲池大小
    int page_pool_size_;
    //需要进行替换的page的id的一个指针
    int replace_pointer_;
    //初始化函数，仅在构造函数中调用
    void initialize(int page_num);
    //提供（返回）一个可以用于存放数据的page的序号
    int offerPageID();
    //将磁盘的file_name文件的block_id块载入缓冲池的第page_id个page中；载入成功返回1，载入失败返回0
    int loadDiskBlock2Page(int page_id, std::string file_name, int block_id);
};


#endif