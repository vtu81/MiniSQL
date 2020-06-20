# API模块的demo程序

编译命令：

```bash
g++ ./API_demo.cpp ./API.cpp ./Index_Manager.cpp ./Record_Manager.cpp ./catalog_manager.cc ./Buffer_Manager.cpp ./SingleAttribute.cpp ./Condition.cpp
```

+ 具体demo内容参见`main()`函数所在文件`API_demo.cpp`

+ 目前API模块可以正常工作的接口包括

  ```c++
  //创建表
  bool createTable(std::string table_name, Attribute attribute, int primary, Index index);
  //删除表
  bool dropTable(std::string table_name);
  //创建索引
  void createIndex(string index_name, string table_name, string attribute_name);
  //删除索引
  void dropIndex(string table_name, string index_name);
  ```

+ 而以下5个与record有关的接口还不能正确工作，待修正：

  ```c++
  //展示记录 -- 无条件
  void showRecord(string table_name, vector<string>* attribute_names);
  //展示记录 -- 有条件
  void showRecord(string table_name, vector<string>* attribute_names, vector<Condition>* conditions);
  //插入记录
  void insertRecord(string table_name, vector<string>* record_content);
  //删除记录 -- 无条件
  void deleteRecord(string table_name);
  //删除记录 -- 有条件
  void deleteRecord(string table_name, vector<Condition>* conditions);
  ```

  

