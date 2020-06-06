#include "Buffer_Manager.h"

 void BufferManager::initialize(int page_num)
{
    page_pool_ = new Page[page_num];
    page_pool_size_ = page_num;
//    replace_pointer_ = 0;
}

BufferManager::~BufferManager()
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
	pin_ = 0;
	dirty_ = false;
	reference_ = false;
	valid_ = false;
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

 void Page::setFileName(std::string n)
{
	file_name_ = n;
}

 std::string Page::getFileName()
{
	return file_name_;
}

 void Page::setBlockID(int block)
{
	block_id_ = block;
}

 int Page::getBlockID()
{
	return block_id_;
}

 void Page::setPin(int pin_state)
{
	pin_ = pin_state;
}

 int Page::getPin()
{
	return pin_;
}

 void Page::setDirty(bool dirty_state)
{
	dirty_ = dirty_state;
}

 bool Page::getDirty()
{
	return dirty_;
}

 void Page::setRefer(bool reference_state)
{
	reference_ = reference_state;
}

 bool Page::getRefer()
{
	return reference_;
}

 char *Page::getBuffer()
{
	return content_;
}


/*------------------ new code added below -------------------*/
void BufferManager::markPageDirty(int page_id)
{
	page_pool_[page_id].setDirty(true);
}

int BufferManager::pinPage(int page_id)
{
	int newpin = page_pool_[page_id].getPin()+1;
	page_pool_[page_id].setPin(newpin);
	return newpin;
}

int BufferManager::unpinPage(int page_id)
{
	int newpin = page_pool_[page_id].getPin() -1;
	//���������pinס��״̬
	if (newpin >= 0)    
		page_pool_[page_id].setPin(newpin);
	//���������δpinס��״̬
	else page_pool_[page_id].setPin(0);
	return page_pool_[page_id].getPin();
}

int BufferManager::outputPage(int page_id)
{
	std::string filename = page_pool_[page_id].getFileName();
	int blockid = page_pool_[page_id].getBlockID();
	//����PageΪ��Ч��Page�����ڲ�����������
	if (page_pool_[page_id].getValid())
	{
		FILE* f = fopen(filename.c_str(), "r+");
		if (f == NULL)
		{
			std::cout << "File open fail!" << std::endl;
			return 0;
		}
		fseek(f, PAGESIZE * blockid, SEEK_SET);
		char* buffer = page_pool_[page_id].getBuffer();
		// ���ڴ�ҳ������д����̿�
		fwrite(buffer, PAGESIZE, 1, f);
		// �ر��ļ�
		fclose(f);
		return 1;
	}
	return 0;
}

int BufferManager::loadDiskBlock2Page(int page_id, std::string file_name, int block_id)
{
	// �򿪴����ļ�
	FILE* f = fopen(file_name.c_str(), "r");
	if (f == NULL)
	{
		std::cout << "File not exist!" << std::endl;
		//ʧ�ܷ���
		return 0;                    
	}
	fseek(f, PAGESIZE * block_id, SEEK_SET);
	char* buffer = page_pool_[page_id].getBuffer();
	// ��ȡ��Ӧ���̿鵽�ڴ�ҳ
	fread(buffer, PAGESIZE, 1, f);
	// �ر��ļ�
	fclose(f);
	// ���������ҳ������Ӧ����
	page_pool_[page_id].setFileName(file_name);
	page_pool_[page_id].setBlockID(block_id);
	//�ն�������ݺ��п��ܲ��ᱻ�滻����ס��ֹ�滻
	page_pool_[page_id].setPin(1);
	page_pool_[page_id].setDirty(false);
	page_pool_[page_id].setRefer(true);
	//��Page��Ч
	page_pool_[page_id].setValid(true);
	return 1;
}

char* BufferManager::fetchPage(std::string file_name, int block_id)
{
	int page_id = fetchPageID(file_name,block_id);
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
	return -1;
}