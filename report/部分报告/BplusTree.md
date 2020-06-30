#### 一、实现功能     

​    B+树是平衡树数据结构，通常用于数据库和操作系统的文件系统中，其插入与修改具有较稳定的对数时间复杂度，因为数据访问时磁盘I/O耗时更加显著，B+树可以最大化内部结点的储存数据量，从而显著减少I/O耗时。在文件"BpTree.h"中，用类模板的方式实现了B+树的查询、插入、删除操作，结合"BufferManager"模拟了B+树在磁盘上的应用。



#### 二、程序说明

##### 1. B+节点类

```c++
template <class Key, class Value>
class BpNode // B+树结点类
{
public:
    friend class BpTree<Key, Value>;
public:
    BpNode(int MAX_KEY, bool isLeaf);
    ~BpNode();
private:
    bool isLeaf; // 是否为叶节点
    int key_count; // 目前储存的key数目
    int MAX_KEY; // 至多储存的key数目
    Key* keys; // key数组
    Value* values; // value数组
    BpNode<Key, Value>** ptrs; // 结点指针数组，对于叶节点，ptrs[MAX_KEY]指向右兄弟
};
```

**成员变量**

+ 友元类 ` BpTree<Key, Value>`，使得B+树类可以直接访问节点类的私有成员
+ 节点是否为叶节点 `isLeaf`
+ 该结点目前储存的键值数目 `key_count`
+ 一个结点至多储存的键值数目 `MAX_KEY`
+ 储存的键值数组 `keys`
+ 储存的数据数组 `values`
+ 结点指针数组`ptrs`，对于叶节点，`ptrs[MAX_KEY]`指向其右兄弟



##### 2. B+树类

```c++
template <class Key, class Value>
class BpTree
{
public:
    Value* search(const Key& key) const;
    bool insert(const Key& key, const Value& value);
    bool delete_single(const Key& key);
    BpTree(std::string file_name, int key_size, int MAX_KEY);
    void write_back_to_disk_all();
    BpNode<Key, Value>* get_minNode();
    void read_from_disk_all();
    ~BpTree();
private:
    void split_node(BpNode<Key, Value>* parent, BpNode<Key, Value>* node, const int pos);
    bool insert(BpNode<Key, Value>* node, const Key& key, const Value& value);
    void delete_entry(BpNode<Key, Value>* node, const Key& key);
    BpNode<Key, Value>* search_node(const Key& key);
    int init_file(std::string file_path);
    int get_block_num(std::string file_path);
    void free_entry(BpNode<Key, Value>* root);
private:
    BpNode<Key, Value>* root; // 根结点
    int MAX_KEY; // 至多储存的key数目
    std::string file_name; //对应的索引文件名
    int key_size; //键的大小：int、float直接取sizeof()l；string类为字符串的长度
};
```

**成员变量**

+ 根结点`root`
+ 其中一个结点至多储存的键值数目`MAX_KEY`
+ 对应的索引文件名`file_name`
+ 一个单位键的大小`key_size`

##### 私有成员函数

+ `split_node()`

  进行过满结点的分裂

+ `bool insert(BpNode<Key, Value>* node, const Key& key, const Value& value)`

  把对应的键值/数据对插入指定的结点`node`中

+ `delete_single()`

  递归式地释放B+树结点内存

+ `search_node(const Key& key)`

  如果输入的键值`key`存在，则返回这一键值所在的B+结点；否则，返回存在于B+树中大于这一键值的最小值所在的结点

+ `init_file()`

  根据输入路径初始化文件

+ `get_block_num()`

  获取索引文件的块数

##### 公开成员函数

+ `Value* search(const Key& key)`

  在B+树中查找对应键值，返回指向对应数据的指针，如果不存在，返回`nullptr`

+ `bool insert()`

  插入对应键值/数据对，返回是否插入成功（失败当且仅当该键值已经存在）

+ `bool delete_single()`

  根据输入的键值在B+树中删去对应的键值/数据对

+ `search_node(const Key& key)`

  如果输入的键值`key`存在，则返回这一键值所在的B+结点；否则，返回存在于B+树中大于这一键值的最小值所在的结点

+ `get_minNode()`

  返回B+树底层最左边的结点（即储存了最小键值的结点），方便对全部数据的遍历

+ `read_from_disk_all()`

  从磁盘中读入B+结点，构造B+树

+ `write_back_to_disk_all()`

  把B+树储存的内容全部写入磁盘



