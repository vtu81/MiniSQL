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

inline void Page::setFileName(std::string n)
{
	file_name_ = n;
}

inline std::string Page::getFileName()
{
	return file_name_;
}

inline void Page::setBlockId(int block)
{
	block_id_ = block;
}

inline int Page::getBlockId()
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