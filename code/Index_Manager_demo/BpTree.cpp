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
    BpTree<int, int>* tree = new BpTree<int, int>("test", 3, 7);
    int length = rand() % 1000000;
    int delete_num = length / 100;
    for (int i = 0;i < length; ++i)
    {
        set.insert(i);
        tree->insert(i, i);
    }


    std::set<int>::iterator it;


    /* (1)单key删除 */
    for (int i = 0;i < delete_num; ++i)
    {
        int temp = rand() % (length);
        if (set.find(temp) != set.end())
        { // 成功删除
            set.erase(temp);
            tree->delete_single(temp);
        }
    }

    /* (2)范围删除
    int min = length / 3;
    int max = length / 2;

    bool minEq = true;
    bool maxEq = true;
    // 1. 删除集合中元素
    for (int i = min + !minEq;i <= max - !maxEq; ++i)
    {
        set.erase(set.find(i));
        tree->delete_single(i);
    }


//    std::cout << "intended: " << max - min + 1 << std::endl;

    // 2. 范围删除B+树中元素
//    tree->delete_range(min, max, minEq, maxEq);
    */

    for (int i = 0;i < length; ++i)
    {
        if (set.find(i) != set.end())
        { // 在集合中出现，即没有被删除
            if (tree->search(i) == nullptr)
            { // 没有被删除，但是在B+树中没有被找到
                std::cout << "Wrong B+ Tree!! Not deleted key not found " << i << std::endl;
//                return ;
            }
        }
        else
        { // 应该被删除
            if (tree->search(i) != nullptr)
            {
                std::cout << "Wrong B+ Tree!! Deleted key is found " << i << std::endl;
//                return ;
            }
        }
    }

    std::cout << "Successful B+ Tree!!" << std::endl;
}
