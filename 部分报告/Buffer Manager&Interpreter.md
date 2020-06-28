#### 一、实现功能     

​    具体来说，在buffer manager模块，设计Page和BufferManager对象，并实现相应部分功能。设计相应的demo程序和用于测试和debug。实现如下功能：1.  读取指定的数据到系统缓冲区或将缓冲区中的数据写出到文件。2. 记录缓冲区中各页的状态，如是否被修改过、是否有效等。3. 提供缓冲区页的pin功能，及锁定缓冲区的页，不允许替换出去。(用于替换策略)  4.使用时钟替换算法实现页面替换。

​    而在Interpreter模块中，负责解析SQL语句，并返回相应信息。主要实现了程序流程控制整体框架，即“启动并初始化、接收命令、处理命令、显示命令结果、循环、退出 “流程。同时接收并解释用户输入的命令，生成命令的内部数据结构表示，检查命令的语法正确性和语义正确性，按照要求返回相应信息。

#### 二、Buffer Manager部分

##### 1. Page对象

```c++
class Page 
{
private:
    bool valid_;
	int block_id_;
	std::string file_name_;
	char content_[PAGESIZE];
	int pin_;
	bool dirty_;
	bool reference_;
public:
	Page();
	void initialize();
    void setValid(bool valid);
    bool getValid();
	void setFileName(std::string n);
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

**成员变量**

+ 页内存空间 ` content_[PAGESIZE]`
+ 保存的内容属于的文件名 `file_name_`
+ 保存的内容在其属于的文件中的block id `block_id_`
+ 钉住数目 `pin_`
+ 引用数目 `reference_`
+ 是否为脏块 `dirty_`
+ 是否有效（即该Page的内容是否有意义）`valid_`

**成员函数**

+ 构造、析构`Page();`

+ 初始化 `void initialize()`

+ 设置/返回上述成员变量值的若干函数

  + 设置/返回文件名 `setFileName/getFileName`
  + 设置/返回block id `setBlockID/getBlockID`
  + 设置/返回钉住数目 `setPin/getPin`
  + 设置/返回引用数目 `setRefer/getRefer`
  + 设置/返回脏块标记 `setDirty/getDirty`
  + 设置/返回dirty标记 `setDirty/getDirty`
  + 返回页（内存空间）首地址 `getBuffer`

##### 2. BufferManager对象

```c++
class BufferManager
{
public:
    BufferManager()
    {
        initialize(MAXPAGEPOOLSIZE);
    }
    BufferManager(int page_num)
    {
        initialize(page_num);
    }
    ~BufferManager();
    char* fetchPage(std::string file_name, int block_id);
    int fetchPageID(std::string file_name, int block_id);
    void markPageDirty(int page_id);
    int pinPage(int page_id);
    int unpinPage(int page_id);
    int outputPage(int page_id);
private:
    Page* page_pool_;
    int page_pool_size_;
    void initialize(int page_num);
    int offerPageID();
    int loadDiskBlock2Page(int page_id, std::string file_name, int block_id);
};

```

**成员变量**

+ 缓冲池`page_pool_`
+ 总页数`page_pool_size_`
+ 其余成员（如用于替换策略的变量）

**成员函数**

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

#### 三、Interpreter部分

##### 1. Interpreter模块

```c++
class Interpreter
{
public:
	API *api;
	string filename;
	int interpreter(string s);
	Interpreter() {};
	~Interpreter() {};
private:
	string getWord(string s, int& index);
	string getWord_no_symbol(string s, int& index);
	int string2num(string s);
};
```

**成员变量**

+ API接口 `api`
+ 执行文件名`filename`

**成员函数**

+ 构造函数和析构函数
+ 解析函数(用于语义解析传入的语句) `interpreter`
+ 提取单词或字符 `getWord`
+ 只读取单词忽略标点符号`getWord_no_symbol`

##### 2. 输入说明

+ 输入的所有字符都不区分大小写，在输出时会自动转为小写状态

+ 换行前的语句后面要加至少一个空格/回车。比如：

  `select sno·换行·from student;`是不合理的，而`select sno ·换行·from student;`是合理的

+ 输入语句若需要声明是字符串数据，需要用单引号扩起来，比如`'example_string'`

+ 支持标准的sql语句

  （1）建表

  ```mysql
  create table [表名] {
  		[属性名1] [数据类型1] [可选择说明是否为unique],
  		[属性名2] [数据类型2] [可选择说明是否为unique],
  		... ...
  		primary key (属性名x) [必须存在]
  };
  ```

  其中数据类型有三种：整数类型`int`、浮点数类型`float`、字符串类型`char(n)`

  （2）创建索引

  ```mysql
  create index [索引名] 
  on [表名] ([表中某一属性名]);
  ```

  （3）删除索引

  ```mysql
  drop index [索引名] on [表名];
  ```

  （4）删除表格

  ```mysql
  drop table [表名];
  ```

  （5）select-from语句：对表格所有的元组做投影

  ```mysql
  select [属性名1], [属性名2], [属性名3], ...
  from [表名];
  ```

  （6）select-from-where语句：选出部分表格的元组做投影

  ```mysql
  select [属性名1], [属性名2], [属性名3], ... ...
  from [表名]
  where [条件1] and [条件2] and ... ...
  ```

  其中条件为关于属性的不等式，比如：`sage >= 30`, `sgender <> 'M'`, `sname = 'A'`，并且只支持条件之间的逻辑与操作（即关键字`and`）
  
  如果希望全选，则用`select * `
  
  （7）delete-from语句：全表记录/元组删除
  
  ```mysql
  delete from [表名];
  ```
  
  （8）delete-from-where语句：删除满足条件的记录/元组
  
  ```mysql
  delete from [表名]
  where [条件1] and [条件2] and ... ...
  ```

+ 不同的语句之间以分号`;`作为分隔，在输入sql语句时，也只在碰到分号`;`后才开始执行
+ 可以在运行程序后在命令行直接输入sql语句，或是全部写在文件中`sql.txt`，然后在命令行输入`execfile sql.txt;`后执行

**3.输出说明**

- 如果SQL语句执行成功，执行后会返回成功信息。

  (1) 建表/删除表格

  ```
  Create table [表名] successfully!
  Drop table [表名] successfully!
  ```

  (2)建立索引/删除索引

  ```
  Create index [索引名] on table [表名] successfully!
  Drop index [索引名] on table [表名] successfully!
  ```

  (3)select语句

  若查询结果不为空，则输出第一行为属性名，其余每行表示一条记录。

  ```
  select result: #################
            [属性名]
         [符合条件的记录]
  ################################
  ```

  若查询结果为空

  ```
  select result: #################
         
  ################################
  ```

  (4)insert语句

  ```
  insert record into table [表名] successfully!
  ```

  (5)delete语句

  ```
  
  ```

  (6)quit语句

  ```
  Exit program!
  ```

- 如果SQL语句执行失败，执行后返回失败原因。

  (1) 建表失败可能出现的原因

  ```
  empty table!
  create table syntax error!
  char size overflow!
  unknown data type
  primary key not exist!
  Table [表名] already exists！
  ```

  (2)建索引失败可能出现的原因

  ```
  index not exist error!
  create index syntax error!
  Table [表名] not exists!
  Index [索引名] already exists!
  Indices on table [表名] full!
  Attribute [属性名] not exists
  ```

  (3)删除表或索引失败可能出现的原因

  ```
  empty table!
  index not exist error!
  drop index syntax error!
  Table [表名] not exists!
  Index [索引名] on table [表名] not exists!
  ```

  (4)select语句失败可能出现的原因

  ```
  query on multiple tables is not allowed!
  empty table!
  invalid operator!
  table [表名] not exist！
  attributes being queried not exist!
  ```

  (5)insert语句失败可能出现的原因

  ```
  insert syntax error!
  table [表名] not exist！
  attributes being queried not exist!
  ```

  (6)delete语句失败可能出现的原因

  ```
  delete syntax error!
  ```

  