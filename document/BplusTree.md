### B+树

（1）使用类模板的方式实现，定义B+树时需要定义键值类型`Key`和数据类型`Value`，初始化时需传入数据：

+ `std::string file_name`：B+树对应的（磁盘）索引文件名
+ `int key_size`：B+树中key的大小
  + `int`：`sizeof(int)`
  + `float`：`sizeof(float)`
  + `char(n)`（实际上是`string`）：`n`
+ `int degree`：B+树的度数

构造时会自动检索磁盘中对应的索引文件，如果存在则直接利用其中数据重构B+树；如果不存在就会新建为一棵空树。（该步骤还没有完成）

```c++
BpTree<int, double>* tree = new BpTree<int, double>(std::string file_name, int key_size, int degree);
```

---

（2）支持操作

【1】单Key查询

```c++
/**
* @param searchKey: 搜索码
* @param value: 如果找到，*value赋值为对应的储存值
* @return: 如果不存在，返回大于搜索码的最小键值所在结点（可以认为nullptr的搜索码为正无穷），否则返回该key所在的叶结点
*/
template <class Key, class Value>
BpNode<Key, Value>* BpTree<Key, Value>::search(const Key &searchKey, Value* value)
```

【2】范围查询

```c++
/**
 * @param min: 范围搜索的最小值
 * @param max: 范围搜索的最大值
 * @param minEq: 是否允许取最小值，默认允许
 * @param maxEq: 是否允许取最大值，默认允许
 * @param keys: 返回的搜索结果key数组
 * @param values: 返回的搜索结果values数组
 * @return: 是否有搜索结果
 */
template <class Key, class Value>
bool BpTree<Key, Value>::search_range(const Key &min, const Key &max, std::vector<Key>* keys, std::vector<Value*>* values, bool minEq = true, bool maxEq = true);
```

【3】单Key-Value插入

```c++
/**
 * @param key: 插入键值
 * @param value: 键值对应数据
 * @return: 是否插入成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::insert(const Key &key, Value *const value);
```

【4】单Key删除

```C++
/**
 * @param key: 待删除的单一键值
 * @return bool: 是否删除成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::delete_single(const Key &key);
```

【5】范围删除

```c++
/**
 * @param min: 范围删除的最小值
 * @param max: 范围删除的最大值
 * @param minEq: 是否允许取最小值，默认允许
 * @param maxEq: 是否允许取最大值，默认允许
 * @return: 是否删除成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::delete_range(const Key &min, const Key &max, bool minEq = true, bool maxEq = true);
```

【6】将B+树所有叶子节点的`key`、`value`写至磁盘B+树索引文件进行记录

```c++
template <class Key, class Value>
void BpTree<Key, Value>::write_back_to_disk_all();
```

---

（3）"BpTree.cpp"中实现测试程序，用stl标准集合容器`std::set`测试B+树的插入、删除、批量删除以及查询，结果正确。

---

（4）"BpTree_and_Disk_demo.cpp"中实现B+树（索引）文件的磁盘读写操作；B+树（索引）文件储存在"./database/index/"目录下。其中`int`、`float`类型数据保存对象二进制数据，而`string`类（`char(n)`）直接保存文本信息。文件的每个block与buffer pool中的每个page大小相等，以'#'标志一个block中的内容结束；文件有一个额外的ending block，首字节同样为'#'。