# Index Manager Overview

```c++
class IndexManager
```

## 接口

+ 构造函数

  ```c++
  IndexManager(API* api);
  ```

  + 传入API指针
  + 初始化：调用API模块中的相关函数，根据catalog的index目录得到以往创建过的index file，将以往创建过的索引（在磁盘上有记录）在内存中重构对应的B+树

+ 创建索引（创建B+树）

  ```c++
  //创建索引
  //@file_name：索引文件名（由record manager中的函数生成并传入）
  //@type: 索引的类型
  void createIndex(std::string file_name, int type);
  ```

+ 删除索引（删除B+树）

  ```c++
  //删除索引
  //@file_name：索引文件名（由record manager中的函数生成并传入）
  //@type: 索引的类型
  void dropIndex(std::string file_name, int type);
  ```

+ 根据单键值查找索引

  ```c++
  //单键值搜索
  //@file_name：索引文件名（由record manager中的函数生成并传入）
  //@key: 待搜索的键值
  //@type: 索引的类型
  //return: 返回记录所在的blockID
  int searchIndex(std::string file_name, std::string key, int type);
  ```

+ 插入单键值索引

  ```c++
  //插入单条索引
  //@file_name：索引文件名（由record manager中的函数生成并传入）
  //@key：索引键值
  //@blockID: 索引对应记录所在磁盘文件的block编号（注意，该处文件指磁盘中的记录文件，而非索引文件）
  //@type: 索引的类型
  void insertIndex(std::string file_name, std::string key, int blockID, int type);
  ```

+ 删除单键值索引

  ```c++
  //根据传入键值删除单条索引
  //@file_name：索引文件名（由record manager中的函数生成并传入）
  //@key: 待删除的键值
  //@type: 索引的类型
  void deleteIndexByKey(std::string file_name, std::string key, int type);
  ```

## 接口参数说明

### `type`

+ -1表示`float`
+ 0表示`int`
+ 其余正数n表示`char(n)`；可能要在高层模块对n进行限制（如0~255），不能太长

### `file_name`

+ API模块调用时传入，由Record Manager的`indexFileNameGet()`给出

### `key`

+ 为方便起见，所有传入的key均转换成`std::string`类型，在Index Manager中进一步转换为对应的`float/int/string`类型

## Appendix 类声明

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
    //-1表示数据类型为float
    static const int TYPE_FLOAT = -1;
    //0表示数据类型为int
    static const int TYPE_INT = 0;
    //正数表示数据类型为字符串，且值为字符串长度; 如34表示char(34)

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
    //return: 返回记录所在的blockID
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

