/*
	�������ӷ� ��������̿�
	�����̿��С�൱�ڿ����̿��ַ���󣬼ٶ�һ���̿�ֻ�ܴ��32�����е�ַ
*/

#ifndef FREEBLOCKMANAGE
#define FREEBLOCKMANAGE
#include"superblock.h"
#include"diskRW.h"
#include"extraFun.h"
#include<iostream>
using namespace std;

//��ȫ��ʼ�����൱��ϵͳ��װ,������ָ��
int totBlockInit() {
	int nexFreeBlock = 1+(inodeNum/(blockSz/inodesz));
	//ÿ32��һ�飬�������̿����Ϣ��ŵ�ÿ���һ���У�31���������һ����
	//������һ���ַ
	for (int i = nexFreeBlock; i < totBlock; i += blockStaSize) {
		//char�Ų���144����short��ע���Сת��
		short str[blockSz/2] = { 0 };
		int ls = 0;
		for (int j = i + 1; j < i + blockStaSize; j++) {
			str[ls++] = j;
		}
		//-1��ʾ�������
		if (i + blockStaSize == totBlock) 
			str[ls++] = -1;
		else str[ls++] = i + blockStaSize;
		wrtBlock(i, (char*) str);
	}
	return nexFreeBlock;
}

//��һ���´�ſ����̿���̿�
bool openBlockPack() {
	if (sprblk.freeBlockStr == -1) {
		//û���µĿ����̿������ʧ��
		return 0;
	}
	short str[blockSz/2] = { 0 };
	rdBlock(sprblk.freeBlockStr, (char*)str);
	//ԭ����̿�Ҳ����˿��̿�
	sprblk.freeBlockSta.push(sprblk.freeBlockStr);
	sprblk.freeBlockStr = str[blockStaSize-1];
	for (int i = 0; i < blockStaSize-1; i++) {
		sprblk.freeBlockSta.push(str[i]);
	}
	return 1;
}

//��װһ��װ�����̵Ŀ��̿�
//������ һ���̿�װ31����ַ ʵ����ջ����32����ַ�˲ŻῪʼ��װ ����31���ĵ�ַ�ŵ�
bool closeBlockPack() {
	short str[blockSz / 2] = { 0 };
	//ջ��Ԫ������������������̿��ַ
	int nextBlock = sprblk.freeBlockSta.top();
	sprblk.freeBlockSta.pop();
	//�����̿�Ž�ȥ
	for (int i = 0; i < blockStaSize-1; i++) {
		str[i] = sprblk.freeBlockSta.top();
		sprblk.freeBlockSta.pop();
	}
	//���� �嵽ͷ��
	str[blockStaSize-1] = sprblk.freeBlockStr;
	sprblk.freeBlockStr = nextBlock;
	wrtBlock(nextBlock, (char*)str);
	return 1;
}

//�õ�һ�����п� ���ؿ��п�ı��
int getBlock() {
	if (sprblk.freeBlockSta.empty()) {
		int fg = openBlockPack();
		//ջ�� Ҳû�а� ʧ��
		if (!fg) return -1;
	}
	sprblk.freeBlockNum--;
	int res = sprblk.freeBlockSta.top();
	sprblk.freeBlockSta.pop();
	return res;
}

//�ͷ�һ�����п�
bool freeBlock(int blockId) {
	sprblk.freeBlockNum++;
	sprblk.freeBlockSta.push(blockId);
	if (sprblk.freeBlockSta.size() == blockStaSize) {
		closeBlockPack();
	}
	return 1;
}
#endif