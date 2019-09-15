/*
	成组链接法 管理空闲盘块
	由于盘块大小相当于空闲盘块地址过大，假定一个盘块只能存放32个空闲地址
*/

#ifndef FREEBLOCKMANAGE
#define FREEBLOCKMANAGE
#include"superblock.h"
#include"diskRW.h"
#include"extraFun.h"
#include<iostream>
using namespace std;

//完全初始化，相当于系统重装,返回首指针
int totBlockInit() {
	int nexFreeBlock = 1+(inodeNum/(blockSz/inodesz));
	//每32块一组，将空闲盘块的信息存放到每组第一块中（31）个，最后一个是
	//链表下一块地址
	for (int i = nexFreeBlock; i < totBlock; i += blockStaSize) {
		//char放不下144，用short。注意大小转换
		short str[blockSz/2] = { 0 };
		int ls = 0;
		for (int j = i + 1; j < i + blockStaSize; j++) {
			str[ls++] = j;
		}
		//-1表示链表结束
		if (i + blockStaSize == totBlock) 
			str[ls++] = -1;
		else str[ls++] = i + blockStaSize;
		wrtBlock(i, (char*) str);
	}
	return nexFreeBlock;
}

//打开一个新存放空闲盘块的盘块
bool openBlockPack() {
	if (sprblk.freeBlockStr == -1) {
		//没有新的空闲盘块包，打开失败
		return 0;
	}
	short str[blockSz/2] = { 0 };
	rdBlock(sprblk.freeBlockStr, (char*)str);
	//原存放盘块也变成了空盘快
	sprblk.freeBlockSta.push(sprblk.freeBlockStr);
	sprblk.freeBlockStr = str[blockStaSize-1];
	for (int i = 0; i < blockStaSize-1; i++) {
		sprblk.freeBlockSta.push(str[i]);
	}
	return 1;
}

//封装一个装空闲盘的块盘块
//讲道理 一个盘块装31个地址 实际上栈中有32个地址了才会开始封装 将后31个的地址放到
bool closeBlockPack() {
	short str[blockSz / 2] = { 0 };
	//栈顶元素用来存放其他空闲盘块地址
	int nextBlock = sprblk.freeBlockSta.top();
	sprblk.freeBlockSta.pop();
	//空闲盘块放进去
	for (int i = 0; i < blockStaSize-1; i++) {
		str[i] = sprblk.freeBlockSta.top();
		sprblk.freeBlockSta.pop();
	}
	//链表 插到头上
	str[blockStaSize-1] = sprblk.freeBlockStr;
	sprblk.freeBlockStr = nextBlock;
	wrtBlock(nextBlock, (char*)str);
	return 1;
}

//得到一个空闲块 返回空闲块的编号
int getBlock() {
	if (sprblk.freeBlockSta.empty()) {
		int fg = openBlockPack();
		//栈空 也没有包 失败
		if (!fg) return -1;
	}
	sprblk.freeBlockNum--;
	int res = sprblk.freeBlockSta.top();
	sprblk.freeBlockSta.pop();
	return res;
}

//释放一个空闲块
bool freeBlock(int blockId) {
	sprblk.freeBlockNum++;
	sprblk.freeBlockSta.push(blockId);
	if (sprblk.freeBlockSta.size() == blockStaSize) {
		closeBlockPack();
	}
	return 1;
}
#endif