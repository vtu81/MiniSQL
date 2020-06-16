//
// Created by 邱泽鸿 on 2020/6/7.
//

#ifndef BPLUSTREE_BPTREE_H
#define BPLUSTREE_BPTREE_H
#include <iostream>
#include <vector>
void test_BpTree_main(); // 测试程序



template <class Key, class Value>
class BpTree; // 提前声明B+树类


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
    BpNode<Key, Value>** ptrs; // 结点指针数组，对于叶节点，ptrs[MAX_KEY+1]指向右兄弟
};

template <class Key, class Value>
BpNode<Key, Value>::BpNode(int MAX_KEY, bool isLeaf)
: MAX_KEY(MAX_KEY), isLeaf(isLeaf), key_count(0)
{
    keys = new Key[MAX_KEY+1];
    values = new Value[MAX_KEY+1];
    ptrs = new BpNode<Key, Value>*[MAX_KEY+2];
}

template <class Key, class Value>
BpNode<Key, Value>::~BpNode()
{
    delete keys;
    delete values;
    delete ptrs;
}


template <class Key, class Value>
class BpTree
{
public:
    Value* search(const Key& key) const;
    bool insert(const Key& key, const Value& value);
    bool delete_single(const Key& key);
    BpTree(int MAX_KEY);
    ~BpTree();
private:
    void split_node(BpNode<Key, Value>* parent, BpNode<Key, Value>* node, const int pos);
    bool insert(BpNode<Key, Value>* node, const Key& key, const Value& value);
    void delete_entry(BpNode<Key, Value>* node, const Key& key);
    BpNode<Key, Value>* search_node(const Key& key);
private:
    BpNode<Key, Value>* root; // 根结点
    int MAX_KEY; // 至多储存的key数目
};


/* 将满的结点node分为两半 */
template <class Key, class Value>
void BpTree<Key, Value>::split_node(BpNode<Key, Value> *parent, BpNode<Key, Value> *node, const int pos)
{
    int half = MAX_KEY / 2;
    // 父节点key与指针右移
    for (int i = parent->key_count; i > pos; --i)
    {
        parent->keys[i] = parent->keys[i-1];
        parent->ptrs[i+1] = parent->ptrs[i];
    }
    parent->key_count ++;
    BpNode<Key, Value>* right = new BpNode<Key, Value>(MAX_KEY, node->isLeaf);
    parent->keys[pos] = node->keys[half];
    parent->ptrs[pos+1] = right;

    // 把右半边分给新的右结点
    for (int i = half+1; i < MAX_KEY; ++i)
    {
        right->keys[i - half - 1] = node->keys[i];
        right->values[i - half - 1] = node->values[i];
        right->ptrs[i - half - 1] = node->ptrs[i];
    }
    right->key_count = MAX_KEY - half - 1;
    right->ptrs[right->key_count] = node->ptrs[MAX_KEY];
    node->key_count = half;

    if (node->isLeaf)
    { // 如果分裂的是叶结点，上升的key需要在底层复制一份
        node->key_count ++;
        // 分配右兄弟
        right->ptrs[MAX_KEY] = node->ptrs[MAX_KEY];
        node->ptrs[MAX_KEY] = right;
    }
}


template <class Key, class Value>
Value* BpTree<Key, Value>::search(const Key &key) const
{
    BpNode<Key, Value>* node = root;
    BpNode<Key, Value>* next = root;

    int pos = 0;
    do
    { // 找到第一个不小于搜索码的该结点的key
        node = next;
        for (pos = 0; pos < node->key_count && key > node->keys[pos]; pos++);
        if (pos < node->key_count && node->isLeaf && key == node->keys[pos])
        { // 在叶节点找到了这一key
            return &(node->values[pos]);
        }
        next = node->ptrs[pos];
    }
    while (!node->isLeaf);

    return nullptr;
}

/* 将值对插入以node为根结点的子B+树 */
template <class Key, class Value>
bool BpTree<Key, Value>::insert(BpNode<Key, Value> *node, const Key &key, const Value& value)
{
    int pos = 0;
    // 找到第一个不小于搜索key的位置
    for (pos = 0; pos < node->key_count && node->keys[pos] < key; pos++);
    if (pos < node->key_count && node->isLeaf && node->keys[pos] == key)
    { // 在B+树的叶节点找到相同的key值
        return false; // 插入失败
    }
    if (!node->isLeaf)
    { // 如果不是叶结点
        auto* child = node->ptrs[pos];
        if (child->key_count == MAX_KEY)
        { // 如果子结点已满，则这个子结点分裂
            split_node(node, child, pos);
        }
        if (key <= node->keys[pos] || pos == node->key_count)
        {
            return insert(node->ptrs[pos], key, value);
        }
        else
        {
            return insert(node->ptrs[pos+1], key, value);
        }
    }
    else // 如果是叶结点，则直接将关键字插入keys数组中
    {
        // 值对右移
        for (int i = node->key_count; i > pos; --i)
        {
            node->keys[i] = node->keys[i-1];
            node->values[i] = node->values[i-1];
            node->ptrs[i] = node->ptrs[i-1];
        }
        // 直接插入
        node->keys[pos] = key;
        node->values[pos] = value;
        node->key_count ++;
        // 插入成功
        return true;
    }
}

/* 将值对插入B+树 */
template <class Key, class Value>
bool BpTree<Key, Value>::insert(const Key &key, const Value& value)
{
    if (root->key_count == MAX_KEY)
    { // 分裂根结点
        auto* newRoot = new BpNode<Key, Value>(MAX_KEY, false);
        newRoot->ptrs[0] = root;
        split_node(newRoot, root, 0);
        root = newRoot;
    }
    return insert(root, key, value);
}


template <class Key, class Value>
BpTree<Key, Value>::BpTree(int MAX_KEY)
: MAX_KEY(MAX_KEY)
{
    root = new BpNode<Key, Value>(MAX_KEY, true);
}

template <class Key, class Value>
BpTree<Key, Value>::~BpTree()
{
    if (root != nullptr) delete root;
}


/**
 * 在以node为根的子B+树中删除具有键值key的数据
 * @param node: 子B+树的根结点
 * @param key
 */
template <class Key, class Value>
void BpTree<Key, Value>::delete_entry(BpNode<Key, Value> *node, const Key &key)
{
    int pos = 0;
    for (pos = 0; pos < node->key_count && key > node->keys[pos]; pos++);
    if (pos < node->key_count && node->keys[pos] == key)
    { // 在当前结点找到关键字key
        if (!node->isLeaf)
        { // 在内点找到
            auto* child = node->ptrs[pos];
            if (child->isLeaf)
            { // 孩子是叶节点
                if (child->key_count > MAX_KEY/2)
                { // 直接删除
                    // 替换父节点中的分割关键字
                    node->keys[pos] = child->keys[child->key_count - 2];
                    child->key_count --;
                    return ;
                }
                else
                { // 孩子key数目不过半
                    if (pos > 0) // 孩子有左兄弟
                    {
                        BpNode<Key, Value>* left = node->ptrs[pos-1];
                        if (left->key_count > MAX_KEY/2)
                        { // 左兄弟key数目充足，可以借
                            // 子结点整体右移一位
                            for (int i = child->key_count; i > 0; --i)
                            {
                                child->keys[i] = child->keys[i-1];
                                child->values[i] = child->values[i-1];
                                child->ptrs[i] = child->ptrs[i-1];
                            }
                            child->keys[0] = node->keys[pos-1];
                            child->values[0] = left->values[left->key_count - 1];
                            child->ptrs[0] = left->ptrs[left->key_count - 1];
                            child->key_count ++;
                            left->key_count --;
                            node->keys[pos-1] = left->keys[left->key_count - 1];
                            node->keys[pos] = child->keys[child->key_count - 2];
                        }
                        else
                        { // 和左兄弟合并
                            // 子结点的内容直接并在其左兄弟的右边
                            for (int i = 0;i < child->key_count; ++i)
                            {
                                left->keys[left->key_count + i] = child->keys[i];
                                left->values[left->key_count+i] = child->values[i];
                                left->ptrs[left->key_count + i] = child->ptrs[i];
                            }
                            left->key_count += child->key_count;
                            // 更新右兄弟结点
                            left->ptrs[MAX_KEY] = child->ptrs[MAX_KEY];

                            // 释放child结点占用的空间node->ptrs[pos];
                            for (int i = pos-1; i < node->key_count-1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                            node->keys[pos-1] = left->keys[left->key_count - 2];
                            pos--;
                        }
                    }
                    else
                    { // 只有右兄弟结点
                        auto* right = node->ptrs[pos+1];
                        if (right->key_count > MAX_KEY/2)
                        { // 右兄弟key数目充足，可以借给子结点child
                            // 更新分割子结点和其右兄弟的key
                            node->keys[pos] = right->keys[0];
                            // 把右兄弟最左边的值对借到子结点最右边
                            child->keys[child->key_count] = right->keys[0];
                            child->values[child->key_count] = right->values[0];
                            child->ptrs[child->key_count] = right->ptrs[0];
                            child->key_count ++;
                            // 右兄弟值对整体左移
                            for (int i = 0;i < right->key_count - 1; ++i)
                            {
                                right->keys[i] = right->keys[i+1];
                                right->values[i] = right->values[i+1];
                                right->ptrs[i] = right->ptrs[i+1];
                            }
                            right->key_count --;
                        }
                        else
                        { // 子结点和右兄弟合并
                            // 把右兄弟的内容直接放在子结点的右半部分
                            for (int i = 0;i < right->key_count; ++i)
                            {
                                child->keys[child->key_count + i] = right->keys[i];
                                child->values[child->key_count + i] = right->values[i];
                                child->ptrs[child->key_count + i] = right->ptrs[i];
                            }
                            child->key_count += right->key_count;
                            // 更新右兄弟指针
                            child->ptrs[MAX_KEY] = right->ptrs[MAX_KEY];

                            // 释放有兄弟占用的空间node->ptrs[i+1];
                            for (int i = pos; i < node->key_count-1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                        }
                    }
                }
            }
            else // 孩子也是内点
            {
                // 找到key在B+树叶节点左兄弟，用存在于这个结点中的前继key值取代key的位置
                BpNode<Key, Value>* last = search_node(key);
                node->keys[pos] = last->keys[last->key_count - 2];
                if (child->key_count > MAX_KEY / 2)
                { // 下一层再处理

                }
                else
                { // 否则孩子结点关键字不过半，需要借或合并
                    if (pos > 0)
                    { // 子结点有左兄弟
                        auto* left = node->ptrs[pos-1];
                        if (left->key_count > MAX_KEY/2)
                        { // 左兄弟key数目充足，可以借给子结点
                            // 子结点整体右移一位
                            for (int i = child->key_count; i > 0; --i)
                            {
                                child->keys[i] = child->keys[i-1];
                                child->ptrs[i+1] = child->ptrs[i];
                            }
                            child->ptrs[1] = child->ptrs[0];
                            // 父节点分隔的key值下沉
                            child->keys[0] = node->keys[pos-1];
                            // 左兄弟最右边的指针赋给子结点
                            child->ptrs[0] = left->ptrs[left->key_count];
                            child->key_count ++;
                            // 左兄弟最后的结点上升，作为新的分隔两兄弟的key值
                            node->keys[pos-1] = left->keys[left->key_count-1];
                            left->key_count --;
                        }
                        else // 需要和左兄弟进行合并
                        {
                            // 父节点分隔两兄弟的key值下沉，赋给左兄弟
                            left->keys[left->key_count] = node->keys[pos-1];
                            left->key_count ++;
                            // 子结点的内容直接放到左兄弟的右边
                            for (int i = 0;i < child->key_count; ++i)
                            {
                                left->keys[left->key_count + i] = child->keys[i];
                                left->ptrs[left->key_count + i] = child->ptrs[i];
                            }
                            left->ptrs[left->key_count + child->key_count] = child->ptrs[child->key_count];
                            left->key_count += child->key_count;

                            // 释放child结点的储存空间node->ptrs[pos];
                            for (int i = pos-1; i < node->key_count - 1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                            pos --;
                        }
                    }
                    else
                    { // 否则子结点只有右兄弟
                        auto* right = node->ptrs[pos+1];
                        if (right->key_count > MAX_KEY/2)
                        { // 右兄弟key数目充足，可以借给子结点
                            // 父节点分隔的key值下沉，赋给子结点
                            child->keys[child->key_count] = node->keys[pos];
                            child->key_count ++;
                            // 右兄弟最左边的指针借到子结点最右边
                            child->ptrs[child->key_count] = right->ptrs[0];
                            // 右兄弟最小的key上升，作为新的分隔两兄弟的key值
                            node->keys[pos] = right->keys[0];
                            int i;
                            for (i = 0;i < right->key_count - 1; ++i)
                            {
                                right->keys[i] = right->keys[i+1];
                                right->ptrs[i] = right->ptrs[i+1];
                            }
                            right->ptrs[i] = right->ptrs[i+1];
                            right->key_count --;
                        }
                        else
                        { // 需要和右兄弟合并
                            child->keys[child->key_count] = node->keys[pos];
                            child->key_count ++;
                            // 将右兄弟的值放到子结点右边
                            for (int i = 0;i < right->key_count; ++i)
                            {
                                child->keys[child->key_count + i] = right->keys[i];
                                child->ptrs[child->key_count + i] = right->ptrs[i];
                            }
                            child->ptrs[child->key_count + right->key_count] = right->ptrs[right->key_count];

                            child->key_count += right->key_count;
                            for (int i = pos; i < node->key_count-1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                        }
                    }
                }
            }
            delete_entry(node->ptrs[pos], key);
        }
        else
        { // 在叶节点找到，直接删除
            for (int i = pos; i < node->key_count - 1; ++i)
            {
                node->keys[i] = node->keys[i+1];
                node->values[i] = node->values[i+1];
                node->ptrs[i] = node->ptrs[i+1];
            }
            node->key_count --;
            return ;
        }
    }
    else // 在当前结点没有找到key
    {
        if (!node->isLeaf)
        { // 关键字可能在以子结点为根的子B+树中出现
            auto* child = node->ptrs[pos];
            if (!child->isLeaf)
            { // 子结点是内点
                if (child->key_count > MAX_KEY/2)
                {
                    // 到下一层处理
                }
                else
                { // 子结点关键字数目不过半，则借点或合并
                    if (pos > 0)
                    { // 子结点存在左兄弟
                        auto* left = node->ptrs[pos-1];
                        if (left->key_count > MAX_KEY/2)
                        { // 左兄弟key数目充足，可以借给子结点
                            for (int i = child->key_count; i > 0; --i)
                            { // 内点不需要转移value
                                child->keys[i] = child->keys[i-1];
                                child->ptrs[i+1] = child->ptrs[i];
                            }
                            child->ptrs[1] = child->ptrs[0];
                            child->keys[0] = node->keys[pos-1];
                            child->ptrs[0] = left->ptrs[left->key_count];
                            child->key_count ++;
                            node->keys[pos-1] = left->keys[left->key_count - 1];
                            left->key_count --;
                        }
                        else
                        { // 子结点需要和左兄弟合并
                            left->keys[left->key_count] = node->keys[pos-1];
                            left->key_count ++;
                            for (int i = 0;i < child->key_count; ++i)
                            {
                                left->keys[left->key_count + i] = child->keys[i];
                                left->ptrs[left->key_count + i] = child->ptrs[i];
                            }
                            left->ptrs[left->key_count + child->key_count] = child->ptrs[child->key_count];
                            left->key_count += child->key_count;

                            for (int i = pos - 1; i < node->key_count - 1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                            pos --;
                        }
                    }
                    else
                    { // 否则，子结点只存在右兄弟
                        auto* right = node->ptrs[pos+1];
                        if (right->key_count > MAX_KEY/2)
                        { // 右兄弟key数目充足
                            child->keys[child->key_count] = node->keys[pos];
                            child->key_count ++;
                            child->ptrs[child->key_count] = right->ptrs[0];
                            node->keys[pos] = right->keys[0];

                            int i;
                            for (i = 0;i < right->key_count - 1; ++i)
                            {
                                right->keys[i] = right->keys[i+1];
                                right->ptrs[i] = right->ptrs[i+1];
                            }
                            right->ptrs[i] = right->ptrs[i+1];
                            right->key_count --;
                        }
                        else
                        { // 两结点合并
                            child->keys[child->key_count] = node->keys[pos];
                            child->key_count ++;
                            // 右兄弟的值放在子结点右边
                            for (int i = 0;i < right->key_count; ++i)
                            {
                                child->keys[child->key_count + i] = right->keys[i];
                                child->ptrs[child->key_count + i] = right->ptrs[i];
                            }
                            child->ptrs[child->key_count + right->key_count] = right->ptrs[right->key_count];
                            child->key_count += right->key_count;

                            for (int i = pos; i < node->key_count-1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                        }
                    }
                }
            }
            else // 孩子是叶节点
            {
                if (child->key_count > MAX_KEY/2)
                {
                    // 在下一层处理
                }
                else
                {
                    if (pos > 0)
                    { // 子结点有左兄弟
                        auto* left = node->ptrs[pos-1];
                        if (left->key_count > MAX_KEY/2)
                        { // 左兄弟key数目充足，可以借给子结点
                            for (int i = child->key_count; i > 0; --i)
                            {
                                child->keys[i] = child->keys[i-1];
                                child->values[i] = child->values[i-1];
                                child->ptrs[i] = child->ptrs[i-1];
                            }
                            child->keys[0] = node->keys[pos-1];
                            child->values[0] = left->values[left->key_count-1];
                            child->ptrs[0] = left->ptrs[left->key_count-1];
                            child->key_count ++;
                            left->key_count --;
                            node->keys[pos-1] = left->keys[left->key_count - 1];
                        }
                        else
                        { // 子结点和左兄弟合并
                            for (int i = 0; i < child->key_count; ++i)
                            {
                                left->keys[left->key_count + i] = child->keys[i];
                                left->values[left->key_count + i] = child->values[i];
                                left->ptrs[left->key_count + i] = child->ptrs[i];
                            }
                            left->key_count += child->key_count;
                            left->ptrs[MAX_KEY] = child->ptrs[MAX_KEY];
                            for (int i = pos-1; i < node->key_count - 1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                            pos --;
                        }
                    }
                    else // 否则只有右兄弟
                    {
                        auto* right = node->ptrs[pos+1];
                        if (right->key_count > MAX_KEY/2)
                        { // 右兄弟key数目充足，可以借给子结点
                            node->keys[pos] = right->keys[0];
                            child->keys[child->key_count] = right->keys[0];
                            child->values[child->key_count] = right->values[0];
                            child->ptrs[child->key_count] = right->ptrs[0];
                            child->key_count ++;
                            for (int i = 0;i < right->key_count - 1; ++i)
                            {
                                right->keys[i] = right->keys[i+1];
                                right->values[i] = right->values[i+1];
                                right->ptrs[i] = right->ptrs[i+1];
                            }
                            right->key_count --;
                        }
                        else
                        { // 两结点合并
                            for (int i = 0;i < right->key_count; ++i)
                            {
                                child->keys[child->key_count + i] = right->keys[i];
                                child->values[child->key_count+i] = right->values[i];
                                child->ptrs[child->key_count + i] = right->ptrs[i];
                            }
                            child->key_count += right->key_count;
                            child->ptrs[MAX_KEY] = right->ptrs[MAX_KEY];

                            for (int i = pos; i < node->key_count - 1; ++i)
                            {
                                node->keys[i] = node->keys[i+1];
                                node->ptrs[i+1] = node->ptrs[i+2];
                            }
                            node->key_count --;
                        }
                    }
                }
            }
            delete_entry(node->ptrs[pos], key);
        }
    }
}

template <class Key, class Value>
BpNode<Key, Value>* BpTree<Key, Value>::search_node(const Key &key)
{
    BpNode<Key, Value>* node = root;
    BpNode<Key, Value>* next = root;

    int pos = 0;
    do
    { // 找到第一个不小于搜索码的该结点的key
        node = next;
        for (pos = 0; pos < node->key_count && key > node->keys[pos]; ++pos);
        if (pos < node->key_count && node->isLeaf && key == node->keys[pos])
        { // 在叶节点找到了这一key
            return node;
        }
        next = node->ptrs[pos];
    }
    while (!node->isLeaf);

    return nullptr;
}



template <class Key, class Value>
bool BpTree<Key, Value>::delete_single(const Key &key)
{
    if (this->search(key) == nullptr)
    { // 不存在这一key
        return false;
    }
    delete_entry(root, key);
    if (!root->isLeaf && root->key_count == 0)
    { // 如果删除关键字后，根结点是内点，且只剩一个子结点
        root = root->ptrs[0];
    }

    return true;
}
#endif //BPLUSTREE_BPTREE_H
