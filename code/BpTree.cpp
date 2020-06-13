//
// Created by 邱泽鸿 on 2020/6/7.
//

#include "BpTree.h"
#include <set>


// 测试时需要把BpTree构造器中的read_from_disk_all注释掉
void test_BpTree_main()
{
    std::srand(time(nullptr));
    std::set<int> set;
    BpTree<int, int>* tree = new BpTree<int, int>("sdd", 2, 3);
    int length = rand() % 1000000;
    int delete_num = rand() % 100000;
    for (int i = 0;i < length; ++i)
    {
        set.insert(i);
        tree->insert(i, i);
    }

    /* (1)单key删除 */
    for (int i = 0;i < delete_num; ++i)
    {
        int temp = rand() % (length / 3);
        if (temp == set.erase(temp))
        { // 成功删除，
            tree->delete_single(temp);
        }
    }

    /* (2)范围删除 */
    int min = length / 3;
    int max = length / 2;

    bool minEq = true;
    bool maxEq = true;
    // 1. 删除集合中元素
    for (int i = min + !minEq; i <= max - !maxEq; ++i)
    {
        set.erase(i);
    }
    // 2. 范围删除B+树中元素
    tree->delete_range(min, max, minEq, maxEq);


    std::set<int>::iterator it;
    for (it = set.begin(); it != set.end(); ++it)
    {
        if (!tree->search(*it))
        {
            std::cout << "Wrong B+ Tree!!" << *it << std::endl;
            return ;
        }
    }
    std::cout << "Successful B+ Tree!!" << std::endl;
}