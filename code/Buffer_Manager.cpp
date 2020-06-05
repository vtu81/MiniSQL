#include "Buffer_Manager.h"

inline void BufferManager::initialize(int page_num)
{
    page_pool_ = new Page[page_num];
    page_pool_size_ = page_num;
    replace_pointer_ = 0;
}

void BufferManager::~BufferManager()
{
    for(int i = 0; i < page_pool_size_; i++)
        outputPage(i);
    delete[] page_pool_;
}

Page::Page() {
	initialize();
}

void Page::initialize() {
	file_name_ = "";
	block_id_ = -1;         
	pin_ = false;
	dirty_ = false;
	reference_ = false;
	for (int i = 0; i < PAGESIZE; i++)
		content_[i] = '\0';
}

void Page::setValid(bool valid)
{
	valid_ = valid;
}

bool Page::getValid()
{
	return valid_;
}

inline void Page::setFileName(std::string n)
{
	file_name_ = n;
}

inline std::string Page::getFileName()
{
	return file_name_;
}

inline void Page::setBlockID(int block)
{
	block_id_ = block;
}

inline int Page::getBlockID()
{
	return block_id_;
}

inline void Page::setPin(int pin_state)
{
	pin_ = pin_state;
}

inline int Page::getPin()
{
	return pin_;
}

inline void Page::setDirty(bool dirty_state)
{
	dirty_ = dirty_state;
}

inline bool Page::getDirty()
{
	return dirty_;
}

inline void Page::setRefer(bool reference_state)
{
	reference_ = reference_state;
}

inline bool Page::getRefer()
{
	return reference_;
}

inline char *Page::getBuffer()
{
	return content_;
}


/*------------------ new code added below -------------------*/

char* BufferManager::fetchPage(std::string file_name, int block_id)
{
	int page_id = fetchPageID();
	//������ڻ���أ��Ͱ������뻺���
	if(page_id == PAGENOTEXIST)
	{
		page_id = offerPageID();
		loadDiskBlock2Page(page_id, file_name, block_id);
	}
	page_pool_[page_id].setRefer(true); //������ã��滻���Ի��һ��ʹ��Reference����
	return page_pool_[page_id].getBuffer(); //�����׵�ַ
}

int BufferManager::fetchPageID(std::string file_name, int block_id)
{
	//���������Ѱ�Ҷ�Ӧ��page
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(page_pool_[i].getValid()\
		&& page_pool_[i].getFileName() == file_name\
		&& page_pool_[i].getBlockID() == block_id)
		{
			//�ҵ��ˣ�
			return i;
		}
	}
	//���򷵻�PAGENOTEXIST
	return PAGENOTEXIST;
}

int BufferManager::offerPageID()
{
	int backupPageID = -1;
	//���������Ѱ�ҿ��е�page
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getValid())
		{
			//�ҵ���û��ռ�õ�page
			return i;
		}
		else if(!page_pool_[i].getDirty() && !page_pool_[i].getRefer())
		{
			//ʱ���û��ĵ�һ�α�����ͬʱ���У�
			//�������ÿ����ռ�õ�page�����dirty_��reference_��Ϊfalse
			//���ȱ����������պ�����õ�
			backupPageID = i;
		}
	}
	//����ÿ��page����valid�ģ���Ҫ�滻һ��page��ȥ
	//����ʱ���û������岽�����£�
	//��һ�ε��������Ѱ��dirty_��reference_��Ϊfalse��page����ǰ���Ѿ�˳������ˣ�
	if(backupPageID != -1)
	{
		return backupPageID;
	}
	//���û���ҵ��ٽ��еڶ��ε��������
	//��������page��reference_��Ϊ0��Ѱ�ҵ���һ��reference_=0��page�������ǵڶ��α�����
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getRefer())
		{
			//��page��reference_=0��OK
			return i;
		}
		page_pool_[i].setRefer(false); //��Ϊ0
	}
	//�������û���ҵ�����Ҫ���page
	//�����ε��������Ѱ��dirty_��reference_��Ϊfalse��page���͵�һ��һ����
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getDirty() && !page_pool_[i].getRefer())
		{
			//ʱ���û��ĵ����α���
			//����ÿ����ռ�õ�page�����dirty_��reference_��Ϊfalse
			//ֱ�ӷ���
			return i;
		}
	}
	//�������û���ҵ�����Ҫ���page
	//���Ĵε��������Ѱ�ҵ���һ��reference_=0��page���͵ڶ������ƣ�
	//��Ҳ�����һ�α�����--��Ȼ���ҵ�
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getRefer())
		{
			//��page��reference_=0��OK
			return i;
		}
	}
}