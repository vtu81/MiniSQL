# Buffer Manager Overview

## **Buffer Manager**

> 外部创建`BufferManager`的实例并进行调用

```c++
class BufferManager
```

**成员变量**

+ 缓冲池`page_pool_`
+ 总页数`page_pool_size_`
+ 其余成员（如用于替换策略的变量）

**成员函数**

> 只应该直接调用第一级列表中的函数！

+ 构造`BufferManager()`：调用`initialize()`
  + `initialize()`：初始化
+ 析构`~BufferManager()`：在程序结束时调用`flushPage()`将所有page写回磁盘
  + `loadDiskBlock2Page()`：将指定文件中的指定block载入指定的page

+ `fetchPage()`：返回文件中的某一block在缓冲池中的page首地址；如果不在buffer pool则要载入后返回
  + `fetchPageID()`：找到文件中的某一block在缓冲池中的page id
  + `offerPageID()`：找到一个闲置的页；可能要实行时钟替换策略进行替换
    + `loadDiskBlock2Page()`：将指定文件中的指定block载入指定的page

+ `markPageDirty()`：标记page_id被修改了，即设为dirty
+ `outputPage()`：将缓冲池中的指定页写到磁盘文件中的指定块
  + `loadDiskBlock2Page()`：将指定文件中的指定block载入指定的page
+ `pinPage()`、`unpinPage()`：钉住页面(pin_count++)、解除页面钉住状态(pin_count--)。防止页被替换！

## Page

> 该类不应当在外部调用，由`BufferManager`类的实例间接控制！

```c++
class Page
```

**成员变量**

+ 页内存空间`content_[PAGESIZE]` (其中`PAGESIZE`为常数4096 即4K)
+ 保存的内容属于的文件名 `file_name_`
+ 保存的内容在其属于的文件中的`block_id_` 
  1. 通过`block_id_`*`PAGESIZE`计算偏移量读取磁盘
  2. 初始化的`block_id_`为-1
+ 是否钉住 `pin_`
+ 用于替换策略的变量 `reference_`
+ 是否是脏块`dirty_`

**成员函数**

+ 构造、析构

+ `initialize()`：初始化

+ 设置/返回上述成员变量值的若干函数

  + 设置/返回文件名 `setFileName/getFileName`
  + 设置/返回block id `setBlockID/getBlockID`
  + 设置/返回钉住状态 `setPin/getPin`
  + 设置/返回引用状态 `setDirty/getDirty`
  + 设置/返回dirty标记 `setRefer/getRefer`
  + 返回页（内存空间）首地址`getBuffer()`

## Appendix 类声明

```c++
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
    //将file_name的所有page写到磁盘上
    void outputFile(std::string file_name);

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
	
	void setBlockID(int block_id);
	int getBlockID();

	void setPin(int pin);
	int getPin();

	void setDirty(bool dirty);
	bool getDirty();

	void setRefer(bool reference);
	bool getRefer();

	char *getBuffer();
};
```

