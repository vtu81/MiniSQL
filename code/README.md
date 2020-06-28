# MiniSQL程序运行说明

编译命令：

【Windows】（g++编译器）

```bash
g++ .\main.cpp .\Interpreter.cpp .\API.cpp .\Index_Manager.cpp .\Record_Manager.cpp .\catalog_manager.cc .\Buffer_Manager.cpp .\SingleAttribute.cpp .\Condition.cpp
```

【MacOS】（clang++编译器）

```bash
clang++ main.cpp Interpreter.cpp API.cpp Index_Manager.cpp Record_Manager.cpp catalog_manager.cc Buffer_Manager.cpp SingleAttribute.cpp Condition.cpp
```

+ 如果编译失败，请检查你的g++/clang++版本/支持的C++标准

+ 具体demo内容参见`main()`函数所在文件`API_demo.cpp`

+ 目前`IndexManager`的构造函数有一部分不能正常工作，需待`CatalogManager::GetAllTable()`相关代码调整后再进一步debug，故现在每次创建的索引在磁盘上记录后不能在下次启动数据库时重新读取，敬请留意！

+ API模块的接口包括，可能仍存在若干问题，有待排查

  ```c++
  //创建表
  bool createTable(std::string table_name, Attribute attribute, int primary, Index index);
  //删除表
  bool dropTable(std::string table_name);
  //创建索引
  void createIndex(string index_name, string table_name, string attribute_name);
  //删除索引
  void dropIndex(string table_name, string index_name);
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

  

