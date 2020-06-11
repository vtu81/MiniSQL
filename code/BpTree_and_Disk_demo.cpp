#include "BpTree.h"
using namespace std;

int main()
{
    string namei = "TEST_INT";
    string names = "TEST_STRING";
    string namef = "TEST_FLOAT";
    //初始化string类datas容器
    vector<string> datas;
    datas.push_back("hello");
    datas.push_back("world");
    datas.push_back("nihao");
    datas.push_back("shijie");
    //初始化float类dataf容器
    vector<float> dataf;
    dataf.push_back(1.33434);
    dataf.push_back(0.8);
    dataf.push_back(3.75);
    dataf.push_back(1234.13532);
    //初始化int类datai容器
    vector<int> datai;
    datai.push_back(23);
    datai.push_back(54);
    datai.push_back(632);
    datai.push_back(12490);

    vector<int> BLOCK_ID;
    BLOCK_ID.push_back(4);
    BLOCK_ID.push_back(54);
    BLOCK_ID.push_back(12);
    BLOCK_ID.push_back(85);

    BpTree<int, int> *T = new BpTree<int, int>(namei, sizeof(int), 4096/sizeof(int));
    T->insert(datai[0], &BLOCK_ID[0]);
    T->insert(datai[1], &BLOCK_ID[1]);
    T->insert(datai[2], &BLOCK_ID[2]);
    T->insert(datai[3], &BLOCK_ID[3]);
    T->insert(datai[0], &BLOCK_ID[0]);

    // BpTree<string, int> *T = new BpTree<string, int>(names, 10, 408);
    // T->insert(datas[0], &BLOCK_ID[0]);
    // T->insert(datas[1], &BLOCK_ID[1]);
    // T->insert(datas[2], &BLOCK_ID[2]);
    // T->insert(datas[3], &BLOCK_ID[3]);

    // BpTree<int, int> *T = new BpTree<int, int>(namef, sizeof(float), 4096/sizeof(float));
    // T->insert(dataf[0], &BLOCK_ID[0]);
    // T->insert(dataf[1], &BLOCK_ID[1]);
    // T->insert(dataf[2], &BLOCK_ID[2]);
    // T->insert(dataf[3], &BLOCK_ID[3]);
    T->write_back_to_disk_all();
    delete T;

    BufferManager bm;
    char* p = bm.fetchPage("./database/index/" + namei, 0);
    int result, value;
    readDataFromMem(p, result, sizeof(int));
    cout<<"first key: "<<result<<endl;
    p += sizeof(int);
    readDataFromMem(p, value, sizeof(int));
    cout<<"first value: "<<value<<endl;
    p += sizeof(int);

    readDataFromMem(p, result, sizeof(int));
    cout<<"second key: "<<result<<endl;
    p += sizeof(int);
    readDataFromMem(p, value, sizeof(int));
    cout<<"second value: "<<value<<endl;
    p += sizeof(int);

    readDataFromMem(p, result, sizeof(int));
    cout<<"third key: "<<result<<endl;
    p += sizeof(int);
    readDataFromMem(p, value, sizeof(int));
    cout<<"third value: "<<value<<endl;
    p += sizeof(int);

    readDataFromMem(p, result, sizeof(int));
    cout<<"fourth key: "<<result<<endl;
    p += sizeof(int);
    readDataFromMem(p, value, sizeof(int));
    cout<<"fourth value: "<<value<<endl;
    p += sizeof(int);
    return 0;
}