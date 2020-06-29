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
	//如果本身处于pin住的状态
	if (newpin >= 0)    
		page_pool_[page_id].setPin(newpin);
	//如果本身处于未pin住的状态
	else page_pool_[page_id].setPin(0);
	return page_pool_[page_id].getPin();
}

int BufferManager::outputPage(int page_id)
{
	std::string filename = page_pool_[page_id].getFileName();
	int blockid = page_pool_[page_id].getBlockID();
	//若此Page为有效的Page，即内部内容有意义
	if (page_pool_[page_id].getValid())
	{
		page_pool_[page_id].setValid(false);
		FILE* f = fopen(filename.c_str(), "r+");
		if (f == NULL)
		{
			std::cout << "File " << filename << " failed to open!" << std::endl;
			return 0;
		}
		fseek(f, PAGESIZE * blockid, SEEK_SET);
		char* buffer = page_pool_[page_id].getBuffer();
		// 将内存页的内容写入磁盘块
		fwrite(buffer, PAGESIZE, 1, f);
		// 关闭文件
		fclose(f);
		return 1;
	}
	return 0;
}

void BufferManager::outputFile(std::string file_name)
{
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(page_pool_[i].getFileName() == file_name)
			outputPage(i);
	}
}

int BufferManager::loadDiskBlock2Page(int page_id, std::string file_name, int block_id)
{
	// 打开磁盘文件
	FILE* f = fopen(file_name.c_str(), "r");
	if (f == NULL)
	{
		std::cout << "File " << file_name << " not exists!" << std::endl;
		//失败返回
		return 0;                    
	}
	fseek(f, PAGESIZE * block_id, SEEK_SET);
	char* buffer = page_pool_[page_id].getBuffer();
	// 读取对应磁盘块到内存页
	fread(buffer, PAGESIZE, 1, f);
	// 关闭文件
	fclose(f);
	// 对新载入的页进行相应设置
	page_pool_[page_id].setFileName(file_name);
	page_pool_[page_id].setBlockID(block_id);
	//刚读入的数据很有可能不会被替换，钉住防止替换
	page_pool_[page_id].setPin(1);
	page_pool_[page_id].setDirty(false);
	page_pool_[page_id].setRefer(true);
	//该Page有效
	page_pool_[page_id].setValid(true);
	return 1;
}

char* BufferManager::fetchPage(std::string file_name, int block_id)
{
	int page_id = fetchPageID(file_name,block_id);
	//如果不在缓冲池，就把它载入缓冲池
	if(page_id == PAGENOTEXIST)
	{
		page_id = offerPageID();
		loadDiskBlock2Page(page_id, file_name, block_id);
	}
	page_pool_[page_id].setRefer(true); //标记引用，替换策略会进一步使用Reference属性
	return page_pool_[page_id].getBuffer(); //返回首地址
}

int BufferManager::fetchPageID(std::string file_name, int block_id)
{
	//遍历缓冲池寻找对应的page
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(page_pool_[i].getValid()\
		&& page_pool_[i].getFileName() == file_name\
		&& page_pool_[i].getBlockID() == block_id)
		{
			//找到了！
			return i;
		}
	}
	//否则返回PAGENOTEXIST
	return PAGENOTEXIST;
}

int BufferManager::offerPageID()
{
	int backupPageID = -1;
	//遍历缓冲池寻找空闲的page
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getValid())
		{
			//找到了没被占用的page
			return i;
		}
		else if(!page_pool_[i].getDirty() && !page_pool_[i].getRefer())
		{
			//时钟置换的第一次遍历（同时进行）
			//否则对于每个被占用的page，如果dirty_、reference_都为false
			//就先保存下来，日后可能用到
			backupPageID = i;
		}
	}
	//否则每个page都是valid的，需要替换一个page出去
	//采用时钟置换。具体步骤如下：
	//第一次单向遍历：寻找dirty_、reference_都为false的page（在前面已经顺便完成了）
	if(backupPageID != -1)
	{
		return backupPageID;
	}
	//如果没有找到再进行第二次单向遍历：
	//将遍历的page的reference_置为0，寻找到第一个reference_=0的page（下面是第二次遍历）
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getRefer())
		{
			//该page的reference_=0，OK
			return i;
		}
		page_pool_[i].setRefer(false); //置为0
	}
	//如果还是没有找到符合要求的page
	//第三次单向遍历：寻找dirty_、reference_都为false的page（和第一次一样）
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getDirty() && !page_pool_[i].getRefer())
		{
			//时钟置换的第三次遍历
			//对于每个被占用的page，如果dirty_、reference_都为false
			//直接返回
			return i;
		}
	}
	//如果还是没有找到符合要求的page
	//第四次单向遍历：寻找到第一个reference_=0的page（和第二次相似）
	//这也是最后一次遍历了--必然能找到
	for(int i = 0; i < page_pool_size_; i++)
	{
		if(!page_pool_[i].getRefer())
		{
			//该page的reference_=0，OK
			return i;
		}
	}
	return -1;
}