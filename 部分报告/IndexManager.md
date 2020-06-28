#### 一、实现功能

Index Manager作为B+树的驱动模块，负责索引创建、索引删除、键值插入、键值删除、键值等值查找。Index Manager掌管3张由索引文件名映射到B+树地址的`std::map`，分别对应键值为`int`、`float`、`string`(即`char(n)`)类型。API模块通过`IndexManager`对象可以间接地创建、删除或找到索引对应的B+树，再进行上述索引相关的操作。利用Index Manager，还可以实现索引文件在磁盘上的记录与读取，即"永久"索引。





#### 二、程序说明

Index Manager模块的相关的代码主要在"IndexManager.h"和"IndexManager.cpp"中，而索引文件的在磁盘的记录代码则放到了B+树模块中。

##### `IndexManager`类

```c++
class IndexManager
{
private:
    //传入的API指针
    API* api_;
    //定义类型：索引文件名 -> B+树 
    typedef std::map<std::string, BpTree<int, int>*> INTMAP;
    //定义类型：索引文件名 -> B+树 
    typedef std::map<std::string, BpTree<float, int>*> FLOATMAP;
    //定义类型：索引文件名 -> B+树 
    typedef std::map<std::string, BpTree<std::string, int>*> STRINGMAP;
    //映射 索引文件名 -> int键B+树
    INTMAP int_index_map_;
    //映射 索引文件名 -> float键B+树
    FLOATMAP float_index_map_;
    //映射 索引文件名 -> string键B+树
    STRINGMAP string_index_map_;
    

public:
    //0表示数据类型为float
    static const int TYPE_FLOAT = 0;
    //-1表示数据类型为int
    static const int TYPE_INT = -1;
    //正数表示数据类型为字符串，且值为字符串长度 + 1; 如34表示char(33)

    //构造函数
    //利用API间接调用catalog manager获取各表已有的index，在磁盘上找到对应的文件重构B+树
    IndexManager(API* api);
    
    //析构函数
    //将已有的所有B+树写到磁盘上，释放内存
    ~IndexManager();

    //创建索引
    //@file_name：索引文件名（由record manager中的函数生成并传入）
    //@type: 索引的类型
    void createIndex(std::string file_name, int type);
    
    //删除索引
    //@file_name：索引文件名（由record manager中的函数生成并传入）
    //@type: 索引的类型
    void dropIndex(std::string file_name, int type);
    
    //单键值搜索
    //@file_name：索引文件名（由record manager中的函数生成并传入）
    //@key: 待搜索的键值
    //@type: 索引的类型
    //return: 
    //找到的话返回记录所在blockID
    //找不到key则返回-1
    //没有该index返回-2
    int searchIndex(std::string file_name, std::string key, int type);
    
    //（不要求范围搜索，故暂时不实现）多键值范围搜索
    // std::vector<int>* searchIndexRange();

    //插入单条索引
    //@file_name：索引文件名（由record manager中的函数生成并传入）
    //@key：索引键值
    //@blockID: 索引对应记录所在磁盘文件的block编号（注意，该处文件指磁盘中的记录文件，而非索引文件）
    //@type: 索引的类型
    void insertIndex(std::string file_name, std::string key, int blockID, int type);

    //根据传入键值删除单条索引
    //@file_name：索引文件名（由record manager中的函数生成并传入）
    //@key: 待删除的键值
    //@type: 索引的类型
    void deleteIndexByKey(std::string file_name, std::string key, int type);
};
```

**成员变量**

+ API类指针`api_`：用于调用API模块中的相关函数`API::allIndexInfoGet()`，以初始化所有磁盘上的索引至内存中（重构对应的B+树）
+ 索引文件名至B+树的3个（不同类型键值的）`std::map`，包括`int_index_map_`、`float_index_map_`和`string_index_map_`；B+树的两个模板参数类型`<key, value>`分别为`<int, int>`、`<float, int>`和`<string, int>`，其中`key`参数类型表示键值的数据类型，而**`value`参数类型表示该键值对应的记录所在记录文件的blockID**（即用记录在record file中的block编号来找到索引对应的记录所在位置）
+ 静态常量`TYPE_FLOAT = 0`、`TYPE_INT = -1`，对应用于表示键值为`int`和`float`的`type`数值

**成员函数**

+ `IndexManager(API* api)`

  `IndexManager`的构造函数，传入一个API对象的指针用于初始化成员变量`api_`，再利用API初始化以前创建过并保存在磁盘上的所有索引

+ `~IndexManager()`

  `IndexManager`的析构函数，将3个`std::map`中的B+树写至磁盘上的索引文件中予以保存，释放相关的内存

+ `void createIndex(std::string file_name, int type)`

  创建索引接口，根据传入的索引**文件名**和键值类型，在对应的`std::map`中新增映射关系，并创建一棵新的B+树用于保存索引文件

+ `void dropIndex(std::string file_name, int type)`

  删除索引接口，根据传入的索引**文件名**和键值类型，在对应的`std::map`中删除对应的映射关系，删除内存中用于保存索引文件的B+树并释放其内存

+ `int searchIndex(std::string file_name, std::string key, int type)`

  键值等值查找接口，根据传入的索引**文件名**、键值和键值类型，在对应的`std::map`中找到对应的映射关系，调用该B+树的等值搜索接口找到对应的blockID并返回

+ `void insertIndex(std::string file_name, std::string key, int blockID, int type)`

  键值插入接口，根据传入的索引**文件名**、键值、记录所在记录文件的block编号、键值类型，在对应的`std::map`中找到对应的映射关系，调用该B+树的插入键值接口

+ `void deleteIndexByKey(std::string file_name, std::string key, int type)`

  键值删除接口，根据传入的索引**文件名**、键值、键值类型，在对应的`std::map`中找到对应的映射关系，调用该B+树的删除键值接口