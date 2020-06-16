# README

该目录为`Index Manager`的测试/展示目录，`main`函数在`Index_Manager_demo.cpp`中。

**Windows**下编译运行命令：

```bash
g++ ./Index_Manager_demo.cpp ./Index_Manager.cpp ./Buffer_Manager.cpp
./a.exe
```

+ 目录下有若干多余模块，只是在文件引用时用到；为方便没有将引用关系清除干净，不理会即可
+ 由于工程其余部分（如`API`）的代码没有完成，为保证`Index Manager`代码能够运行，部分代码与主目录下（`/code/`）的有些许出入，但基本上是一致的
+ 输出中以`[debug]`开头的输出为内部调试的正常输出！真正运行工程时，会在高层模块中保证该类情形不出现；现在只是对这些情形进行展示，无需担心
+ 如果希望重现写索引文件的过程，可以将`./database/index/`下的所有索引文件的内容清空，重新运行demo程序即可（不要删除对应的文件/目录）