# 可扩展哈希实验

## 说明

1. 数据库课程期末大项目，用c语言实现数据库中的可扩展哈希。
2. 整个项目分成两个部分, 一是读入数据建立索引, 二是读取测试数据获得查询结果.
3. 实现过程中参考了很多github上的实现，吸收了许多有用的想法。
4. 涉及到了许多东西：指针的使用(void与int, float,char的转化与存储)、变长记录的实现(我的做法是在字符串前用4个字节的int类型来储存字符串的长度)、文件IO操作(fwrite, fread, fseek等)、如何简洁地实现读写数据项等，更详细的信息看src目录的源代码以及report.doc。




## 项目模块描述

### main.c

整个项目的入口文件, 该文件通过引用work模块执行操作.对于main.c来说, 只有work模块是”可见的”, main.c 通过work模块来调用项目其他的模块.

### work.h work.c

工作模块, 调用项目中的各个模块来执行项目的两个部分: 建立哈希索引以及查询.

相当于一个顶层模块: 其他模块为具体的功能模块.

哈希索引（`<hash_value, bucket_id>`键值对）输出到hashindex.out文件。

查询结果按partkey属性域排序，输出到testoutput.out文件中。

### buffer_manager.h buffer_manager.c

缓冲区管理模块. 缓冲区的页数为8或者128, 每个页大小为8K.

使用时钟页面置换算法来管理缓冲区. 当其他模块请求一个页的时候,该模块从缓冲区选择一个页,从磁盘装载数据(如果需要的话),然后将该请求的可用的缓冲页返回.

### IO_manager.h IO_manager.c

IO管理模块: 与IO操作有关的函数定义在此模块.

分为两部分: 一是从磁盘装载数据进内存和从内存写回数据到磁盘, 二是与哈希桶分裂有关操作: 目录加倍和哈希桶分裂.

目录加倍将global_depth左移一位并设置正确`<hash_value, bucket_id>`键值对.

哈希桶分裂创建一个新的页面, 顺序读取原来页面的数据, 获取新的哈希值然后重新插入,时数据分布正确.

### page.h page.c

定义了与页面有关的操作.

1. 对于bucket: 获取和设置页面信息, 如局部深度,数据量,空间地址起始位置的偏移值.
2. 对于index: 可储存2048个bucket_id.

### record.h record.c

定义与record有关的操作:

1. record与字符串的相互转换
2. 将record写入page、从page中读取record
3. 获得record键值、大小
4. 判断page存放某个record后是否溢出

### hash.h hash.c

根据键值key获取哈希值

### setting.h setting.c

存放项目运行所需的一些配置信息。

### tracker.h tracker.c

统计信息的模块：IO操作次数、分裂哈希桶次数、加倍目录次数、建立哈希和查找的运行时间。



## 可执行程序

    least_8: 缓冲区页数为8, 低位扩展哈希实现
    least_128: 缓冲区页数为128, 低位扩展哈希实现


## 程序运行方法

    Usage: db [path to lineitem.tbl]
    eg: ./least_128 ./data
