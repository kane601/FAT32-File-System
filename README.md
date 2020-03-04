# FAT32-File-System

该github项目通过实现一个FAT32文件遍历工具来完成对FAT32文件系统结构的解析。

希望能够让我们理解FAT32文件系统的数据存储结构，通过实现一个简易FAT32文件系统目录遍历工具验证对该文件系统的理解是否正确。 

1、学习目标
  1）重点学习理解FAT32文件系统。

  2）读取磁盘数据分析FAT32分区目录结构，编程实现类似windows的文件浏览器工具，需要显示的信息有文件的占用空间、大小、修改时间等。

2、工具效果：该工具目标为实现类似于Windows系统对FAT32磁盘操作，所以详细功能请对照Windows系统操作步骤，具体效果参加下图1。

3、支持功能：
 
  1）在文件列表中，文件夹项支持“双击”进入下一级目录操作。

  2）目录树，支持“单击”切换文件夹，“双击”实现节点的展开与收缩。
