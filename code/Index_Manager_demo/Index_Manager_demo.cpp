#include "Index_Manager.h"
using namespace std;

class API{};
API* api = new API();
IndexManager* im = new IndexManager(api);

int main()
{
    cout << "/************** Begin test for INT **************/" << endl;
    im->createIndex("./database/index/TEST_INT", IndexManager::TYPE_INT);
    //�ҵ��Ļ���ӡ��¼����blockID
    //�Ҳ���key���ӡ-1
    //û�и�index��ӡ-2
    cout << "Record with key value 632 in block " << im->searchIndex("./database/index/TEST_INT", "632", IndexManager::TYPE_INT) << endl;
    cout << "Record with key value 10086 in block " << im->searchIndex("./database/index/TEST_INT", "10086", IndexManager::TYPE_INT) << endl;
    cout << "Record with key value 23 in block " << im->searchIndex("./database/index/TEST_INT", "23", IndexManager::TYPE_INT) << endl;
    cout << "Record with key value 39 in block " << im->searchIndex("./database/index/TEST_INT", "39", IndexManager::TYPE_INT) << endl;
    cout << "Record with key value 101 in block " << im->searchIndex("./database/index/TEST_INT", "101", IndexManager::TYPE_INT) << endl;
    cout << im->searchIndex("./database/index/WHATEVER", "632", IndexManager::TYPE_INT) << endl;

    im->deleteIndexByKey("./database/index/TEST_INT", "23", IndexManager::TYPE_INT);

    cout << "Record with key value 23: " << im->searchIndex("./database/index/TEST_INT", "23", IndexManager::TYPE_INT) << endl;

    im->insertIndex("./database/index/TEST_INT", "10086", 3, IndexManager::TYPE_INT);
    im->insertIndex("./database/index/TEST_INT", "23", 5, IndexManager::TYPE_INT);
    im->insertIndex("./database/index/TEST_INT", "39", 34, IndexManager::TYPE_INT);
    im->insertIndex("./database/index/TEST_INT", "101", 99, IndexManager::TYPE_INT);
    im->insertIndex("./database/index/TEST_INT", "632", 298, IndexManager::TYPE_INT);

    im->dropIndex("./database/index/TEST_INT", IndexManager::TYPE_INT); //dropIndexֻ���ڴ����ͷ��˶�Ӧ�Ŀռ䣬���Ὣ�ļ��Ӵ����Ƴ�����Ҫ��API/RM�н�һ��ɾ��

    cout << "Record with key value 101: " << im->searchIndex("./database/index/TEST_INT", "101", IndexManager::TYPE_INT) << endl;

    /*--------------------------------------------------------------------------------------------------------------------------------*/
    cout << "/************** Begin test for FLOAT **************/" << endl;
    im->createIndex("./database/index/TEST_FLOAT", IndexManager::TYPE_FLOAT);

    im->insertIndex("./database/index/TEST_FLOAT", "13.24", 0, IndexManager::TYPE_FLOAT);
    im->insertIndex("./database/index/TEST_FLOAT", "1.01", 254, IndexManager::TYPE_FLOAT);
    im->insertIndex("./database/index/TEST_FLOAT", "399.0", 757, IndexManager::TYPE_FLOAT);

    cout << "Record with key value 13.24 in block " << im->searchIndex("./database/index/TEST_FLOAT", "13.24", IndexManager::TYPE_FLOAT) << endl;
    cout << "Record with key value 1.01 in block " << im->searchIndex("./database/index/TEST_FLOAT", "1.01", IndexManager::TYPE_FLOAT) << endl;
    cout << "Record with key value 399.0 in block " << im->searchIndex("./database/index/TEST_FLOAT", "399.0", IndexManager::TYPE_FLOAT) << endl;

    /*--------------------------------------------------------------------------------------------------------------------------------*/
    cout << "/************** Begin test for STRING **************/" << endl;
    im->createIndex("./database/index/TEST_STRING", 10);

    im->insertIndex("./database/index/TEST_STRING", "Hello", 956, 10);
    im->insertIndex("./database/index/TEST_STRING", "World", 14, 10);
    im->insertIndex("./database/index/TEST_STRING", "!!!", 356, 10);
    im->insertIndex("./database/index/TEST_STRING", "1234567890", 666, 10);

    cout << "Record with key value \'Hello\' in block " << im->searchIndex("./database/index/TEST_STRING", "Hello", 10) << endl;
    cout << "Record with key value \'World\' in block " << im->searchIndex("./database/index/TEST_STRING", "World", 10) << endl;
    cout << "Record with key value \'!!!\' in block " << im->searchIndex("./database/index/TEST_STRING", "!!!", 10) << endl;
    cout << "Record with key value \'1234567890\' in block " << im->searchIndex("./database/index/TEST_STRING", "1234567890", 10) << endl;

    delete im;
    return 0;
}