# MiniSQL程序运行说明

编译命令：

【Windows】（g++编译器）

（1）运行自定义的sql语句

```bash
g++ -o main.exe .\main.cpp .\Interpreter.cpp .\API.cpp .\Index_Manager.cpp .\Record_Manager.cpp .\catalog_manager.cpp .\Buffer_Manager.cpp .\SingleAttribute.cpp .\Condition.cpp
```

（2）运行现成的测试集

```bash
g++ -o test.exe .\test.cpp .\Interpreter.cpp .\API.cpp .\Index_Manager.cpp .\Record_Manager.cpp .\catalog_manager.cpp .\Buffer_Manager.cpp .\SingleAttribute.cpp .\Condition.cpp
```



【MacOS】（clang++编译器）

（1）运行自定义的sql语句

```bash
clang++ -o main.out main.cpp Interpreter.cpp API.cpp Index_Manager.cpp Record_Manager.cpp catalog_manager.cpp Buffer_Manager.cpp SingleAttribute.cpp Condition.cpp
```

（2）运行现成的测试集

```bash
clang++ -o test.out .\test.cpp .\Interpreter.cpp .\API.cpp .\Index_Manager.cpp .\Record_Manager.cpp .\catalog_manager.cpp .\Buffer_Manager.cpp .\SingleAttribute.cpp .\Condition.cpp
```

