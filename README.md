# MiniSQL

浙江大学数据库系统期末作业







###B+树

（1）使用类模板的方式实现，定义B+树时需要定义键值类型Key和数据类型Value，初始化时无需输入数据。

```c++
BpTree<int, double>* tree = new BpTree<int, double>();
```

（2）支持操作

【1】单Key查询

```c++
/**
 * @param searchKey: 搜索码
 * @param pos: 如果码已经存在，则为码的位置，否则为大于该搜索码的最小码的位置
 * @return bool: 是否存在
 */
template <class Key, class Value>
bool BpNode<Key, Value>::search(const Key &searchKey, int &pos);
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

（3）"BpTree.cpp"中实现测试程序，用stl标准集合容器std::set测试B+树的插入、删除、批量删除以及查询，结果正确。

