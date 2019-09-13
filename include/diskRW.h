#ifndef DISKRW
#define DISKRW

#include<iostream>
#include<fstream>
#include"inode.h"
#include"superblock.h"
#include"menu.h"
using namespace std;

//写超级块
bool wrtSpblk(Sprblk sprblk) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	iofile.seekg(ios::beg);
	iofile.write((char*)&sprblk, sizeof(Sprblk));
	iofile.close();
	return 1;
}

//读超级块
Sprblk rdSpblk() {
	Sprblk res;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	iofile.seekg(ios::beg);
	iofile.read((char*)&res, sizeof(Sprblk));
	iofile.close();
	return res;
}

//写Inode节点
bool wrtInode(int inodeid, Inode cur) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//超级块+块号+i节点号
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.write((char*)&cur, sizeof(Inode));
	iofile.close();
	return 1;
}

//二进制写Inode
bool wrtInodeInb(int inodeid, char s[inodesz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//超级块+块号+i节点号
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.write(s, inodesz);
	iofile.close();
	return 1;
}

//写文件块
bool wrtBlock(int blockid, char s[blockSz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//重定向至该块的起始位置
	iofile.seekg(ios::beg + blockid * blockSz);
	//写
	iofile.write(s, blockSz);
	iofile.close();
	return 1;
}

//读Inode节点
Inode rdInode(int inodeid) {
	Inode res;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//超级块+块号+i节点号
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.read((char*)&res, sizeof(Inode));
	iofile.close();
	return res;
}

//二进制读Inode
void rdInodeInb(int inodeid, char s[inodesz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//重定向至该块的起始位置
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	//读
	iofile.read(s, inodesz);
	iofile.close();
	return;
}

//写目录到文件块
bool wrtBlock(int blockid, Menu menu) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//重定向至该块的起始位置
	iofile.seekg(ios::beg + blockid * blockSz);
	//写
	iofile.write((char*)&menu, sizeof(Menu));
	iofile.close();
	return 1;
}

//读文件块
void rdBlock(int blockid, char s[blockSz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//重定向至该块的起始位置
	iofile.seekg(ios::beg + blockid * blockSz);
	//读
	iofile.read(s, blockSz);
	iofile.close();
	return ;
}

//读文件块的目录
Menu rdBlockMenu(int blockid) {
	Menu menu;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//重定向至该块的起始位置
	iofile.seekg(ios::beg + blockid * blockSz);
	//读
	iofile.read((char*)&menu, sizeof(Menu));
	iofile.close();
	return menu;
}


//注意！字符串输出最后要有/0 但作为文件块不需要/0；
#endif