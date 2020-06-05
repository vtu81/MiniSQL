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
	//���
	int block_id_;
	//�ļ���
	std::string file_name_;
	//Page����
	char content_[PAGESIZE];
	//pin������
	int pin_;
	//�Ƿ�Ϊ���
	bool dirty_;
	//�����滻���Ա���
	bool reference_;
public:
	//Ĭ�Ϲ��캯��
	Page();
	//��ʼ������
	void initialize();
	// get��set����
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
    //Ĭ�Ϲ��캯��
    BufferManager()
    {
        initialize(MAXPAGEPOOLSIZE);
    }
    //����page_num���й���
    BufferManager(int page_num)
    {
        initialize(page_num);
    }
    //����������output����ҳ
    ~BufferManager();
    //����file_name�ļ���block_id����page_pool_�е��׵�ַ
    char* fetchPage(std::string file_name, int block_id);
    //����file_name�ļ���block_id����page_pool_��page���
    int fetchPageID(std::string file_name, int block_id);
    //��ǵ�page_id��pageΪdirty
    int markPageDirty(int page_id);
    //��ס��page_id��page��pin_count++�����ظı���pin_count
    int pinPage(int page_id);
    //�����page_id��page�Ķ�ס״̬��pin_count--�����ظı���pin_count
    int unpinPage(int page_id);
    //��page_pool_�еĵ�page_id��pageд�����̣�����1��ʾ����ɹ�������0��ʾ���ʧ��
    int outputPage(int page_id);
    //int outputPage(int page_id, std::string file_name, int block_id);

private:
    //������׵�ַָ��
    Page* page_pool_;
    //����ش�С
    int page_pool_size_;
    //��Ҫ�����滻��page��id��һ��ָ��
    int replace_pointer_;
    //��ʼ�����������ڹ��캯���е���
    void initialize(int page_num);
    //�ṩ�����أ�һ���������ڴ�����ݵ�page�����
    int offerPageID();
    //�����̵�file_name�ļ���block_id�����뻺��صĵ�page_id��page�У�����ɹ�����1������ʧ�ܷ���0
    int loadDiskBlock2Page(int page_id, std::string file_name, int block_id);
};


#endif