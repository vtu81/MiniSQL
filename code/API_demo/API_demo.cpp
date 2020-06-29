#include "Catalog_Manager.h"
#include "Index_Manager.h"
#include "Record_Manager.h"
#include "API.h"

#include <iostream>

BufferManager buffer_manager;
API api;
CatalogManager catalog_manager(buffer_manager);
RecordManager record_manager;
IndexManager index_manager(&api);




int main(int argc,char * argv[])
{
    //因为构造3个manager的时候用到了api，因此在此处才初始化api的3个manager指针
    api.cm = &catalog_manager;
    api.rm = &record_manager;
    api.im = &index_manager;
    //初始化record_manager
    record_manager.api = &api;
    record_manager.bm = &buffer_manager;
    record_manager.cm = &catalog_manager;

    index_manager.init();

    Attribute attr;
    attr.num = 5;
    attr.name[0] = "SID";
    attr.name[1] = "SNAME";
    attr.name[2] = "CID";
    attr.name[3] = "CNAME";
    attr.name[4] = "SCORE";
    attr.isindex[0] = false;
    attr.isindex[1] = false;
    attr.isindex[2] = false;
    attr.isindex[3] = false;
    attr.isindex[4] = false;
    attr.primarykey = 0;
    attr.repeat[0] = true;
    attr.repeat[1] = false;
    attr.repeat[2] = false;
    attr.repeat[3] = false;
    attr.repeat[4] = false;
    attr.type[0] = 10;
    attr.type[1] = 20;
    attr.type[2] = -1;
    attr.type[3] = 20;
    attr.type[4] = 0;
    Index ind;
    ind.num = 0;

    cout << "You are testing API module!" << endl;
    cout << "[operation]API::createTable(STUDENT) starts" << endl;
    api.createTable("STUDENT", attr, 0, ind);
    cout << "[operation]API::createTable(STUDENT) ends" << endl;

    cout << "[operation]API::createIndex(...) starts" << endl;
    api.createIndex("SIDINDEX", "STUDENT", "SID");
    cout << "[operation]API::createIndex(...) ends" << endl;

    cout << "[operation]API::insertRecord(...) starts" << endl;
    vector<string> test_tuple1;
    test_tuple1.push_back("123");
    test_tuple1.push_back("Tom");
    test_tuple1.push_back("456");
    test_tuple1.push_back("DB");
    test_tuple1.push_back("98.6");
    api.insertRecord("STUDENT", &test_tuple1);
    vector<string> test_tuple2;
    test_tuple2.push_back("189");
    test_tuple2.push_back("Jack");
    test_tuple2.push_back("663");
    test_tuple2.push_back("ADS");
    test_tuple2.push_back("80.0");
    api.insertRecord("STUDENT", &test_tuple2);
    vector<string> test_tuple3;
    test_tuple3.push_back("256");
    test_tuple3.push_back("Abby");
    test_tuple3.push_back("999");
    test_tuple3.push_back("OS");
    test_tuple3.push_back("60.3");
    api.insertRecord("STUDENT", &test_tuple3);
    cout << "[operation]API::insertRecord(...) ends" << endl;

    cout << "[operation]API::showRecord(...) starts" << endl;
    vector<string> attr_name;
    attr_name.push_back("SID");
    attr_name.push_back("SNAME");
    attr_name.push_back("CID");
    attr_name.push_back("CNAME");
    attr_name.push_back("SCORE");
    api.showRecord("STUDENT", &attr_name);
    cout << "[operation]API::showRecord(...) ends" << endl;

    cout << "[operation]API::showRecord(\"STUDENT\", NULL) starts" << endl;
    api.showRecord("STUDENT", NULL);
    cout << "[operation]API::showRecord(\"STUDENT\", NULL) ends" << endl;

    cout << "[operation]API::showRecord(... with conditions) starts" << endl;
    vector<Condition> conditions;
    Condition c("SID", "256", Condition::OPERATOR_EQUAL);
    conditions.push_back(c);
    api.showRecord("STUDENT", &attr_name, &conditions);
    cout << "[operation]API::showRecord(... with conditions) ends" << endl;

    cout << "[operation]API::deleteRecord(...) starts" << endl;
    vector<Condition> test_vec_cond;
    Condition test_cond("SID", "123", Condition::OPERATOR_MORE);
    test_vec_cond.push_back(test_cond);
    api.deleteRecord("STUDENT", &test_vec_cond);
    //api.deleteRecord("STUDENT");
    cout << "[operation]API::deleteRecord(...) ends" << endl;

    cout << "[operation]API::showRecord(...) starts" << endl;
    api.showRecord("STUDENT", &attr_name);
    cout << "[operation]API::showRecord(...) ends" << endl;

    // cout << "[operation]API::createIndex(...) starts" << endl;
    // api.createIndex("SIDINDEX", "STUDENT", "SID");
    // cout << "[operation]API::createIndex(...) ends" << endl;

    cout << "[operation]API::dropIndex(...) starts" << endl;
    api.dropIndex("STUDENT", "SIDINDEX");
    cout << "[operation]API::dropIndex(...) ends" << endl;
    
    cout << "[operation]API::dropTable(XXX) starts" << endl;
    api.dropTable("XXX");
    cout << "[operation]API::dropTable(XXX) ends" << endl;
    
    cout << "[operation]API::dropTable(STUDENT) starts" << endl;
    api.dropTable("STUDENT");
    cout << "[operation]API::dropTable(STUDENT) ends" << endl;

    return 0;
}