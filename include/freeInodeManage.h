#ifndef FREEINODEMANAGE
#define FREEINODEMANAGE

/*
成组链接法 管理空闲inode
由于盘块大小相当于空闲盘块地址过大，假定一个盘块只能存放20个空闲地址
*/

#include"superblock.h"
#include"diskRW.h"
#include"extraFun.h"
#include"freeBlockManage.h"
#include<iostream>
using namespace std;

//完全初始化，相当于系统重装,返回首指针
int totInodeInit() {
	int nexFreeInode = 0;
	//每20块一组，将空闲盘块的信息存放到每组第一块中（19）个，最后一个是
	//链表下一块地址
	for (int i = 0; i < inodeNum; i += inodeStaSize) {
		//char放不下144，用short。注意大小转换
		short str[blockSz / 8] = { 0 };
		int ls = 0;
		for (int j = i + 1; j < i + inodeStaSize; j++) {
			str[ls++] = j;
		}
		str[ls] = i + inodeStaSize;
		//-1表示链表结束
		if (i + inodeStaSize == inodeNum) str[ls] = -1;
		wrtInodeInb(i, (char*)str);
	}
	return nexFreeInode;
}

//打开一个新存放空闲盘块的盘块
bool openInodePack() {
	if (sprblk.freeInodeStr == -1) {
		//没有新的空闲盘块包，打开失败
		return 0;
	}
	short str[blockSz / 8] = { 0 };
	rdInodeInb(sprblk.freeInodeStr, (char*)str);
	//原存放盘块也变成了空盘快
	sprblk.freeInodeSta.push(sprblk.freeInodeStr);
	sprblk.freeInodeStr = str[inodeStaSize-1];
	for (int i = 0; i < inodeStaSize-1; i++) {
		sprblk.freeInodeSta.push(str[i]);
	}
	return 1;
}

//封装一个装空闲盘的块盘块
bool closeInodePack() {
	short str[blockSz / 8] = { 0 };
	int nextInode = sprblk.freeInodeSta.top();
	sprblk.freeInodeSta.pop();
	for (int i = 0; i < inodeStaSize-1; i++) {
		str[i] = sprblk.freeInodeSta.top();
		sprblk.freeInodeSta.pop();
	}
	//链表 插到头上
	str[inodeStaSize-1] = sprblk.freeInodeStr;
	sprblk.freeInodeStr = nextInode;
	wrtInodeInb(nextInode, (char*)str);
	return 1;
}

//得到一个空闲块 返回空闲块的编号
//这个编号表示它所对应的磁盘位置是空的（可写的）
int getInode() {
	if (sprblk.freeInodeSta.empty()) {
		int fg = openInodePack();
		//栈空 也没有包 失败
		if (!fg) return -1;
	}
	sprblk.freeInodeNum--;
	int res = sprblk.freeInodeSta.top();
	sprblk.freeInodeSta.pop();
	return res;
}

//释放一个空闲块
bool freeInode(int inodeId) {
	sprblk.freeInodeNum++;
	sprblk.freeInodeSta.push(inodeId);
	if (sprblk.freeInodeSta.size() == inodeStaSize) {
		closeInodePack();
	}
	return 1;
}
#endif

