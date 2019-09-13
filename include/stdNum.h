#ifndef STDNUM
#define STDNUM

//总数目640+（60/4）+1 = 656；
const int totBlock = 656;
const int inodeNum = 60; //i节点总数
const int blockNum = 640; //存文件的盘块数目

//栈的大小
const int blockStaSize = 32;
const int inodeStaSize = 20;
//每块大小（字节）
const int blockSz = 512;
//每个inode的占位 (inode实际大小为72 添加了更多描述时间的变量 包含一些冗余)
const int inodesz = 128;
//每块有几个Inode
const int B_I = 4;
//每级索引大小 <512/4 = 128
const int indexsz = 16;
//字符串长度限制 实际上是11因为\0
const int stdls = 12;

#endif