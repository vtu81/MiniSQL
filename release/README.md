# README

## 编译

使用**make**工具编译**MiniSQL**。

对于Windows用户，需要先安装**mingw32-make**，在当前目录下运行如下命令以编译**MiniSQL**：

```powershell
mingw32-make -B
./MiniSQL.exe
```

对于MacOS/Linux/Unix用户，需要先安装**GNU Make**工具，在当前目录下运行如下命令以编译**MiniSQL**：

```bash
make -B
./MiniSQL
```

注意：

+ 如果"./src"已有".o"文件，请务必先**清除编译文件**再进行以上的步骤！

+ 请保证编译器至少支持C++11标准

+ 默认使用g++进行编译；如果需要使用编译器，请自行修改makefile

+ 如果需要手动编译，在"./src/"目录下的编译命令如下：

  ```bash
  g++ -std=c++11 main.cpp Interpreter.cpp API.cpp Catalog_Manager.cpp Record_Manager.cpp Index_Manager.cpp Buffer_Manager.cpp Condition.cpp SingleAttribute.cpp -o MiniSQL
  ```

## 清除编译文件

对于Windows用户，需要手动删除"./src/"目录中的所有".o"文件，并删除当前目录下的"MiniSQL.exe"。

对于MacOS/Linux/Unix用户，执行如下命令即可：

```bash
make clean
```

## 运行

Windows用户在当前目录下运行**MiniSQL**：

```powershell
./MiniSQL.exe
```

MacOS/Linux/Unix用户在当前目录下运行**MiniSQL**：

```bash
./MiniSQL
```

### MiniSQL运行说明

#### 数据类型

支持3种数据类型：`int`，`float`，`char(n)`；其中 1 ≤ n ≤ 255

#### 表定义

一个表最多可以定义32个属性，各属性可以指定是否为`unique`；必须定义单属性的主键

#### 索引的建立和删除

对于表的主属性自动建立B+树索引，对于声明为`unique`的属性可以通过SQL语句由用户指定建立/删除B+树索引（所有的B+树索引都是单属性单值的）

#### 查找记录

可以通过指定用`and`连接的多个条件进行查询，支持等值查询和区间查询

#### 插入和删除记录

支持每次一条或多条记录的插入操作；支持每次一条或多条记录的删除操作

#### 注意事项

+ 运行后提示"MiniSQL interface"，左边出现`MiniSQL>`提示输入信息
+ **MiniSQL**支持语句逐条输入，每条语句以分号结尾；按下回车后，**MiniSQL**会给予反馈
+ 当前目录下提供了测试用SQL文件"test.sql"，可在运行后通过命令`execfile test.sql`执行文件中的语句
+ 当出现语句错误时，**MiniSQL**会报错"syntax error"
+ 输入`quit;`，即可退出**MiniSQL**；此时程序弹出`Goodbye!`，退出程序
+ 详细介绍参见报告

## 数据库文件及注意事项

**数据库文件**结构如下：

+ ./database
  + ./database/catalog
  + ./database/record/
    + 相关的记录文件
  + ./database/index/
    + 相关的索引文件
+ **请不要自行修改"./database/"目录及目录下的任何东西！**
+ 如果"./database/"目录损坏或丢失文件，请自行创建
  + "./database/"目录
  + "./database/index/"目录
  + "./database/record/"目录

+ "./database/catalog"文件不需要自行创建；但MacOS/Linux/Unix用户在丢失原有的"./database/catalog"文件后首次运行**MiniSQL**时可能会报错导致**MiniSQL**无法正常运行，可以忽略相关报错，再次运行**MiniSQL**即可。
+ 当然，假如"./database/catalog"文件在多次运行**MiniSQL**后仍然没有自动创建，请自行新建空的"./database/catalog"文件，并增加一个字符"#"，作为文件的首字节。