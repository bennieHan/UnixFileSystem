#ifndef SUPERBLOCK
#define SUPERBLOCK
#include"extraFun.h"
#include<iostream>
#include<time.h>
#include"stdNum.h"

using namespace std;

struct Sprblk {
	//盘块总数
	int blockTot;
	//空闲i节点数目+栈+
	int freeInodeNum;
	Stack freeInodeSta;
	//成组链接首块的地址
	int freeInodeStr;
	//空闲盘块数目+栈
	int freeBlockNum;
	Stack freeBlockSta;
	//成组链接首块的地址
	int freeBlockStr;
	//根目录的Inode编号
	int root;
	//修改
	bool changeFlag;
	time_t changeTime;
	bool umask[3][3];
	//用户相关
	char username[5][stdls];
	char password[5][stdls];
	char group[5][stdls];

	Sprblk() {
		//用户清零
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		memset(group, 0, sizeof(group));
		//初始化盘块数目 640+15+1
		blockTot = totBlock;
		freeInodeNum = inodeNum;
		freeBlockNum = blockNum;
		//初始化栈大小
		freeInodeSta.sz = inodeStaSize;
		freeBlockSta.sz = blockStaSize;
		changeFlag = 0;
		changeTime = time(NULL);
		umask[0][0] = 0; umask[0][1] = 0; umask[0][2] = 0;
		umask[1][0] = 0; umask[1][1] = 0; umask[1][2] = 0;
		umask[2][0] = 0; umask[2][1] = 1; umask[2][2] = 0;
	}
}sprblk;

#endif