#ifndef SUPERBLOCK
#define SUPERBLOCK
#include"extraFun.h"
#include<iostream>
#include<time.h>
#include"stdNum.h"

using namespace std;

struct Sprblk {
	//�̿�����
	int blockTot;
	//����i�ڵ���Ŀ+ջ+
	int freeInodeNum;
	Stack freeInodeSta;
	//���������׿�ĵ�ַ
	int freeInodeStr;
	//�����̿���Ŀ+ջ
	int freeBlockNum;
	Stack freeBlockSta;
	//���������׿�ĵ�ַ
	int freeBlockStr;
	//��Ŀ¼��Inode���
	int root;
	//�޸�
	bool changeFlag;
	time_t changeTime;
	bool umask[3][3];
	//�û����
	char username[5][stdls];
	char password[5][stdls];
	char group[5][stdls];

	Sprblk() {
		//�û�����
		memset(username, 0, sizeof(username));
		memset(password, 0, sizeof(password));
		memset(group, 0, sizeof(group));
		//��ʼ���̿���Ŀ 640+15+1
		blockTot = totBlock;
		freeInodeNum = inodeNum;
		freeBlockNum = blockNum;
		//��ʼ��ջ��С
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