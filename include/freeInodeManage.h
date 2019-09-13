#ifndef FREEINODEMANAGE
#define FREEINODEMANAGE

/*
�������ӷ� �������inode
�����̿��С�൱�ڿ����̿��ַ���󣬼ٶ�һ���̿�ֻ�ܴ��20�����е�ַ
*/

#include"superblock.h"
#include"diskRW.h"
#include"extraFun.h"
#include"freeBlockManage.h"
#include<iostream>
using namespace std;

//��ȫ��ʼ�����൱��ϵͳ��װ,������ָ��
int totInodeInit() {
	int nexFreeInode = 0;
	//ÿ20��һ�飬�������̿����Ϣ��ŵ�ÿ���һ���У�19���������һ����
	//������һ���ַ
	for (int i = 0; i < inodeNum; i += inodeStaSize) {
		//char�Ų���144����short��ע���Сת��
		short str[blockSz / 8] = { 0 };
		int ls = 0;
		for (int j = i + 1; j < i + inodeStaSize; j++) {
			str[ls++] = j;
		}
		str[ls] = i + inodeStaSize;
		//-1��ʾ�������
		if (i + inodeStaSize == inodeNum) str[ls] = -1;
		wrtInodeInb(i, (char*)str);
	}
	return nexFreeInode;
}

//��һ���´�ſ����̿���̿�
bool openInodePack() {
	if (sprblk.freeInodeStr == -1) {
		//û���µĿ����̿������ʧ��
		return 0;
	}
	short str[blockSz / 8] = { 0 };
	rdInodeInb(sprblk.freeInodeStr, (char*)str);
	//ԭ����̿�Ҳ����˿��̿�
	sprblk.freeInodeSta.push(sprblk.freeInodeStr);
	sprblk.freeInodeStr = str[inodeStaSize-1];
	for (int i = 0; i < inodeStaSize-1; i++) {
		sprblk.freeInodeSta.push(str[i]);
	}
	return 1;
}

//��װһ��װ�����̵Ŀ��̿�
bool closeInodePack() {
	short str[blockSz / 8] = { 0 };
	int nextInode = sprblk.freeInodeSta.top();
	sprblk.freeInodeSta.pop();
	for (int i = 0; i < inodeStaSize-1; i++) {
		str[i] = sprblk.freeInodeSta.top();
		sprblk.freeInodeSta.pop();
	}
	//���� �嵽ͷ��
	str[inodeStaSize-1] = sprblk.freeInodeStr;
	sprblk.freeInodeStr = nextInode;
	wrtInodeInb(nextInode, (char*)str);
	return 1;
}

//�õ�һ�����п� ���ؿ��п�ı��
//�����ű�ʾ������Ӧ�Ĵ���λ���ǿյģ���д�ģ�
int getInode() {
	if (sprblk.freeInodeSta.empty()) {
		int fg = openInodePack();
		//ջ�� Ҳû�а� ʧ��
		if (!fg) return -1;
	}
	sprblk.freeInodeNum--;
	int res = sprblk.freeInodeSta.top();
	sprblk.freeInodeSta.pop();
	return res;
}

//�ͷ�һ�����п�
bool freeInode(int inodeId) {
	sprblk.freeInodeNum++;
	sprblk.freeInodeSta.push(inodeId);
	if (sprblk.freeInodeSta.size() == inodeStaSize) {
		closeInodePack();
	}
	return 1;
}
#endif

