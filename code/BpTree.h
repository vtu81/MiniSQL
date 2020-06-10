//
// Created by 邱泽鸿 on 2020/6/7.
//

#ifndef BPLUSTREE_BPTREE_H
#define BPLUSTREE_BPTREE_H
#include <iostream>
#include <vector>

void test_BpTree_main(); // used for debug

template <class Key, class Value>
class BpTree; // 提前声明B+树类

template <class Key, class Value>
class BpNode
{
public: // 把B+树类指定为结点的友元类，允许其访问私有成员
    friend class BpTree<Key, Value>;
public:
    BpNode(int degree);
    bool isRoot();
    bool isLeaf();
    bool isFull();
    ~BpNode();
    bool search(const Key& searchKey, int& pos); // 如果码已经存在，则为码的位置，否则为大于该搜索码的最小码的位置
    BpNode<Key, Value>* split(Key& promoted); // key promoted to upper layer
    bool insert(const Key& key, Value* const value = nullptr); // 插入key值，返回是否插入成功（已存在时无法插入）
    bool remove(const int& pos); // 删除对应位置的key与value
private:
    std::vector<Key> keys; //
    std::vector<Value*> values; //
    int count; // number of keys in the node
    BpNode<Key, Value>* rightNode;
    std::vector<BpNode<Key, Value>*> kids;
    BpNode<Key, Value>* parent;
    int degree; // 一个结点包含key值的最大数量
};

template <class Key, class Value>
class BpTree
{
public:
    BpTree(int degree);
    ~BpTree();
    BpNode<Key, Value>* search(const Key& searchKey, Value* value);
    BpNode<Key, Value>* search(const Key& searchKey);
    bool search_range(const Key& min, const Key& max, std::vector<Key>* keys, std::vector<Value*>* values, bool minEq = true, bool maxEq = true);
    bool insert(const Key& key, Value* const value);
    bool delete_single(const Key& key);
    bool delete_range(const Key& min, const Key& max, bool minEq = true, bool maxEq = true);
private:
    BpNode<Key, Value>* root; // B+树的根结点
    BpNode<Key, Value>* minNode; // 最左端的叶节点
    int height; // B+树高度
    int count; // 结点个数
    int degree; // 一个结点包含key值的最大数量
private:
    bool delete_entry(BpNode<Key, Value>* node, const Key& key);
};


template <class Key, class Value>
BpNode<Key, Value>::BpNode(int degree)
: count(0), parent(nullptr), rightNode(nullptr), kids(), keys(), values(), degree(degree)
{
    for (int i = 0; i <= degree; ++i)
    { // 理论上key与value的大小应该为"degree"，
      // 但是为了方便处理，允许结点满时暂时超出阈值大小，所以所有数组数据的大小+1
        kids.push_back(nullptr);
        keys.push_back(Key());
        values.push_back(nullptr);
    }

    kids.push_back(nullptr);
}


template <class Key, class Value>
BpNode<Key, Value>::~BpNode()
{

}

template <class Key, class Value>
bool BpNode<Key, Value>::isRoot()
{
    return (parent == nullptr);
}

template <class Key, class Value>
bool BpNode<Key, Value>::isLeaf()
{
    return (kids[0] == nullptr);
}


// 判断结点是否需要分裂
template <class Key, class Value>
bool BpNode<Key, Value>::isFull()
{
    return (count > degree);
}


/**
 * @param searchKey: 搜索码
 * @param pos: 如果码已经存在，则为码的位置，否则为大于该搜索码的最小码的位置
 * @return bool: 是否存在
 */
template <class Key, class Value>
bool BpNode<Key, Value>::search(const Key &searchKey, int &pos)
{
    if (count == 0 || searchKey < keys[0])
    {
        pos = 0;
        return false;
    }
    if (searchKey > keys[count-1])
    {
        pos = count;
        return false;
    }
    int left = 0;
    int right = count-1;
    while (right - left > 1)
    {
        pos = (right + left) / 2;
        if (keys[pos] == searchKey)
        {
            return true;
        }
        if (keys[pos] < searchKey)
        {
            left = pos;
        }
        else
        {
            right = pos;
        }
    }
    if (searchKey == keys[left])
    {
        pos = left;
        return true;
    }
    if (searchKey == keys[right])
    {
        pos = right;
        return true;
    }
    pos = right;

    return false;
}

template <class Key, class Value>
BpNode<Key, Value>* BpNode<Key, Value>::split(Key &promoted)
{
    int leftSize = (count / 2); // 分裂后原节点的大小
    BpNode<Key, Value>* right = new BpNode<Key, Value>(degree); // 分裂新增的右节点
    promoted = this->keys[leftSize];

    right->parent = this->parent; // 分裂节点具有相同的父节点
    // 更新右节点
    right->rightNode = this->rightNode;
    this->rightNode = right;

    if (this->isLeaf())
    { // 如果节点是叶节点
        for (int i = leftSize; i <= degree; ++i)
        {   // 将右半边的key与value复制到"right"中
            // promoted的key值 也 保留在"right"中
            right->keys[i - leftSize] = this->keys[i];
            right->values[i - leftSize] = this->values[i];
            // 清除原节点对应的key与value
            this->keys[i] = Key();
            this->values[i] = nullptr;
        }
        // 更新节点计数
        right->count = count - leftSize;
        count = leftSize;
    }
    else
    { // 叶子是非叶节点
        for (int i = leftSize+1; i <= degree; ++i)
        {   // 将右半边的key与value复制到"right"中
            // promoted的key值 不 保留在"right"中
            right->keys[i - leftSize - 1] = this->keys[i];
            right->values[i - leftSize - 1] = this->values[i];
            // 清除原节点对应的key与value
            this->keys[i] = Key();
            this->values[i] = nullptr;
        }
        // "promoted"被插入高层后不再在这一层出现
        this->keys[leftSize] = Key();
        this->values[leftSize] = nullptr;

        for (int i = leftSize+1; i <= count; ++i)
        {   // 右半边孩子也要进行转移
            right->kids[i - leftSize - 1] = this->kids[i];
            right->kids[i - leftSize - 1]->parent = right;
            this->kids[i] = nullptr;
        }
        // 更新节点计数
        right->count = count - leftSize - 1;
        count = leftSize;
    }
    return this;
}

template <class Key, class Value>
bool BpNode<Key, Value>::insert(const Key &key, Value* const value)
{
    int pos = 0;
    if (this->search(key, pos))
    { // key值已经存在
        return false;
    }

    // 移动新码右边的key与value
    for (int i = count; i > pos; --i)
    {
        keys[i] = keys[i-1];
        values[i] = values[i-1];
    }
    // 将key值插入结点
    keys[pos] = key;
    // 将value值插入结点
    values[pos] = value;


    if (!this->isLeaf())
    { // 如果为非叶节点，为将要插入中间的孩子预留一个位置（在新结点的右边）
        for (int i = count+1; i > pos+1; --i)
        {
            this->kids[i] = kids[i-1];
        }
        this->kids[pos+1] = nullptr;
    }
    this->count++;
    return true;
}

template <class Key, class Value>
BpTree<Key, Value>::BpTree(int degree)
: count(0), root(nullptr), minNode(nullptr), height(0), degree(degree){}


/**
* @param searchKey: 搜索码
* @param value: 如果找到，value赋值为对应的指针
* @return: 如果不存在，返回大于搜索码的最小键值所在结点（可以认为nullptr的搜索码为正无穷），否则返回该key所在的叶结点
*/
template <class Key, class Value>
BpNode<Key, Value>* BpTree<Key, Value>::search(const Key &searchKey, Value* value)
{
    if (root == nullptr) return nullptr;
    BpNode<Key, Value>* node = root;
    int pos = 0;
    while (!node->isLeaf())
    { // 迭代直至到达叶子层
        if (node->search(searchKey, pos))
        { // 该结点包含这一搜索码
            node = node->kids[pos+1];
        }
        else
        { // 该结点不包含这一搜索码，但是找到了最小的大于搜索码的码之位置
            node = node->kids[pos];
        }
    }

    // 现在node为某一叶节点
    if (node->search(searchKey, pos))
    { // 找到该搜索码
        if (node->values[pos] != nullptr)
        { // 存在储存值
            if (value == nullptr)
            { // 指针没有分配内存
                value = new Value();
            }
            *value = *(node->values[pos]);
        }
        return node;
    }
    else
    {
        value = nullptr;
        return node;
    }
}

/**
 * @param searchKey: 搜索码
 * @param value: 如果找到，value赋值为对应的指针
 * @return: 如果不存在，返回大于搜索码的最小键值所在结点（可以认为nullptr的搜索码为正无穷），否则返回该key所在的叶结点
 */
template <class Key, class Value>
BpNode<Key, Value>* BpTree<Key, Value>::search(const Key &searchKey)
{ // 不需要知道value
    Value* value = nullptr;
    return this->search(searchKey, value);
}

/**
 * @param key: 插入键值
 * @param value: 键值对应数据
 * @return: 是否插入成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::insert(const Key &key, Value *const value)
{
    if (root == nullptr)
    {
        root = new BpNode<Key, Value>(degree);
        root->insert(key, value);
        count++;
        minNode = root;
        return true;
    }
    BpNode<Key, Value>* node = root;
    int pos = 0;
    while (!node->isLeaf())
    { // 迭代直至到达叶子层
        if (node->search(key, pos))
        { // 该结点包含这一搜索码
            node = node->kids[pos+1];
            // 此时底层的码一定在以当前结点为根的子树的最左边
            while (!node->isLeaf())
            {
                node = node->kids[0];
            }
            break;
        }
        else
        { // 该结点不包含这一搜索码，但是找到了最小的大于搜索码的码之位置
            node = node->kids[pos];
        }
    }
    if (!node->insert(key, value))
    { // 插入失败，有重复值
        return false;
    }
    else
    { // 成功插入，需要判断是否进行调整
        while (node->isFull())
        {
            Key promoted;
            node = node->split(promoted);
            count++;
            if (node->isRoot())
            { // 根结点
                height++;
                BpNode<Key, Value>* newRoot = new BpNode<Key, Value>(degree);
                this->root = newRoot;
                node->rightNode->parent = node->parent = root;
                root->insert(promoted);
                root->kids[0] = node;
                root->kids[1] = node->rightNode;
                break;
            }
            else
            {
                BpNode<Key, Value>* parent = node->parent;
                parent->insert(promoted);
                parent->search(promoted, pos);
                parent->kids[pos+1] = node->rightNode;
                node->rightNode->parent = parent;
                node = node->parent;
            }
        }
        return true;
    }
}

/**
 * @param key: 待删除的单一键值
 * @return bool: 是否删除成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::delete_single(const Key &key)
{
    BpNode<Key, Value>* node = this->search(key);
    int temp = 0;
    if (!node->search(key, temp))
    { // 没有找到对应的key
        return false;
    }
    if (count == 1)
    {
        root = nullptr;
        count = 0;
        height = 0;
        minNode = nullptr;
        return true;
    }
    return delete_entry(node, key);
}


template <class Key, class Value>
bool BpNode<Key, Value>::remove(const int &pos)
{
    for (int i = pos; i < count - 1; ++i)
    {
        keys[i] = keys[i+1];
        values[i] = values[i+1];
        kids[i+1] = kids[i+2];
    }
    keys[count - 1] = Key();
    values[count - 1] = nullptr;
    kids[count] = nullptr;
    count --;
    return true;
}

template <class Key, class Value>
bool BpTree<Key, Value>::delete_entry(BpNode<Key, Value> *node, const Key &key)
{
    int pos = 0;
    // 从"node"中删除key
    if (!node->search(key, pos))
    {
        return false;
    }
    node->remove(pos);

    if (node->isRoot() && node->kids[1] == nullptr)
    { // node是根结点
        root = node->kids[0];
        root->parent = nullptr;
        minNode = root;
        count--;
        height--;
    }
    else
    {
        if (node->isRoot()) return true;
        int minNum = 0; // 该结点最少所需的key数目（区分为叶节点和非叶节点）
        if (node->isLeaf())
        {
            minNum = (degree + 1) / 2;
        }
        else
        {
            minNum = degree / 2;
        }
        if (node->count < minNum)
        { // 结点中key太少
            BpNode<Key, Value>* parent = node->parent;
            bool right = true;
            if (parent->search(node->keys[0], pos))
            { // node中最小的键值出现在父节点中
              // 此时返回的角标对应左兄弟
                right = false;
            }
            else
            { // 不出现时，如果不是出现在最右，则取右兄弟，否则左兄弟
                if (pos >= parent->count)
                {
                    right = false;
                    pos--;
                }
                else pos++;
            }
            BpNode<Key, Value>* sibling = parent->kids[pos]; // 左兄弟或右兄弟
            Key parent_key = parent->keys[pos - right]; // 介于指向node与指向sibling两个指针之间的key值
            if (sibling->count > minNum)
            { // 兄弟结点可以借一个key
                if (!right)
                { // 左兄弟
                    if (node->isLeaf())
                    { // 向左兄弟借最后一个值，放在最左边
                        for (int i = node->count; i > 0; --i)
                        { // 整体右移，留出第一个空位
                            node->keys[i] = node->keys[i-1];
                            node->values[i] = node->values[i-1];
                        }
                        node->keys[0] = sibling->keys[sibling->count - 1];
                        node->values[0] = sibling->values[sibling->count - 1];
                        node->count++;
                        // 替换对应父节点key值
                        parent->keys[pos] = node->keys[0];
                        // 删掉左兄弟最后一个值
                        sibling->remove(sibling->count - 1);
                    }
                    else
                    { // 左兄弟最后的key值上升，对应父节点key值下沉，放在node最左边
                        for (int i = node->count; i > 0; --i)
                        { // 整体右移，留出第一个空位
                            node->keys[i] = node->keys[i-1];
                            node->kids[i+1] = node->kids[i];
                        }
                        node->kids[1] = node->kids[0];
                        // 对应父节点key值下沉
                        node->keys[0] = parent_key;
                        // 左兄弟最右边的孩子转为node最左边的孩子
                        node->kids[0] = sibling->kids[sibling->count];
                        // 左兄弟最后的key值上升，替换对应父节点key值
                        parent->keys[pos] = sibling->keys[sibling->count - 1];
                        // 删掉左兄弟最后一个值
                        sibling->remove(sibling->count - 1);
                    }
                }
                else // 右兄弟，和左兄弟的情形对称
                {
                    if (node->isLeaf())
                    { // 向右兄弟借第一个值，放在最右边
                        node->keys[node->count] = sibling->keys[0];
                        node->values[node->count] = sibling->values[0];
                        node->count++;
                        // 替换对应父节点key值
                        parent->keys[pos] = sibling->keys[1];
                        // 删掉左兄弟第一个值
                        sibling->remove(0);
                    }
                    else
                    { // 右兄弟第一个key值上升，对应父节点key值下沉，放在node最右边
                        // 对应父节点key值下沉
                        node->keys[node->count] = parent_key;
                        // 右兄弟最左边的孩子转为node最右边的孩子
                        node->kids[node->count + 1] = sibling->kids[0];
                        // 右兄弟第一个key值上升，替换对应父节点key值
                        parent->keys[pos - 1] = sibling->keys[0];
                        // 删掉右兄弟第一个值
                        sibling->kids[0] = sibling->kids[1];
                        sibling->remove(0);
                    }
                }
                node->count ++;
                return true;
            }
            else
            { // 需要和兄弟合并
                if (right)
                { // "node"在sibling的左边
                    std::swap(node, sibling); // 交换指针
                }
                // 更新兄弟结点
                sibling->rightNode = node->rightNode;

                if (!node->isLeaf())
                { // node不是叶节点
                    // 将parent_key以及node所有指针附加到sibling中
                    sibling->keys[sibling->count] = parent_key;
                    sibling->kids[sibling->count + 1] = node->kids[0];
                    int i = 0;
                    for (i = sibling->count+1; i <= sibling->count + node->count; ++i)
                    {
                        sibling->keys[i] = node->keys[i - sibling->count - 1];
                        sibling->kids[i+1] = node->kids[i - sibling->count];
                    }
                    sibling->count += node->count + 1;
                }
                else
                { // node是叶节点
                    // 只用将node所有的key与value附加到sibling中
                    for (int i = sibling->count; i < sibling->count + node->count; ++i)
                    {
                        sibling->keys[i] = node->keys[i - sibling->count];
                        sibling->values[i] = node->values[i - sibling->count];
                    }
                    sibling->count += node->count;
                }
                return delete_entry(parent, parent_key);
            }
        }
    }
    return true;
}




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
bool BpTree<Key, Value>::search_range(const Key &min, const Key &max, std::vector<Key>* keys, std::vector<Value*>* values, bool minEq, bool maxEq)
{
    BpNode<Key, Value>* node_min = this->search(min);
    BpNode<Key, Value>* node_max = this->search(max);
    int pos_min, pos_max;
    if (!node_min->search(min, pos_min))
    { // 没有相等的最小键值，相当于取大于它最小的键值并且允许最小值取等
        minEq = true;
    }
    if (!node_max->search(max, pos_max))
    { // 没有相等的最大键值，相当于取小于它最大的键值并且允许最大值取等
        maxEq = true;
        pos_max --;
    }

    if (min > max || (min == max && !(minEq && maxEq)))
    { // 最小值大于最小值，或二者相等但指令又不允许取等
        // 搜索结果为空
        return false;
    }
    BpNode<Key, Value>* node = node_min;
    int pos = 0;

    for ( pos = pos_min + !minEq, node = node_min;
          !(node == node_max && pos + !maxEq >= pos_max);
          ++pos
        )
    {
        if (pos >= node->count)
        {
            pos = -1;
            node = node->rightNode;
            if (node == nullptr)
            {
                break;
            }
        }
        else
        {
            keys->push_back(node->keys[pos]);
            values->push_back(node->values[pos]);
        }
    }
    return true;
}

/**
 * @param min: 范围删除的最小值
 * @param max: 范围删除的最大值
 * @param minEq: 是否允许取最小值，默认允许
 * @param maxEq: 是否允许取最大值，默认允许
 * @return: 是否删除成功
 */
template <class Key, class Value>
bool BpTree<Key, Value>::delete_range(const Key &min, const Key &max, bool minEq, bool maxEq)
{
    std::vector<Key>* keys = new std::vector<Key>();
    std::vector<Value*>* values = new std::vector<Value*>();
    if (search_range(min, max, keys, values, minEq, maxEq))
    { // 依次删除keys中元素
        while (!keys->empty())
        {
            delete_single(keys->back());
            keys->pop_back();
//            delete values->back();
//            values->pop_back();
        }
        return true;
    }
    else
    { // 没有符合要求的待删除元素
        return false;
    }
}

#endif //BPLUSTREE_BPTREE_H
