#ifndef DISKRW
#define DISKRW

#include<iostream>
#include<fstream>
#include"inode.h"
#include"superblock.h"
#include"menu.h"
using namespace std;

//д������
bool wrtSpblk(Sprblk sprblk) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	iofile.seekg(ios::beg);
	iofile.write((char*)&sprblk, sizeof(Sprblk));
	iofile.close();
	return 1;
}

//��������
Sprblk rdSpblk() {
	Sprblk res;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	iofile.seekg(ios::beg);
	iofile.read((char*)&res, sizeof(Sprblk));
	iofile.close();
	return res;
}

//дInode�ڵ�
bool wrtInode(int inodeid, Inode cur) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//������+���+i�ڵ��
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.write((char*)&cur, sizeof(Inode));
	iofile.close();
	return 1;
}

//������дInode
bool wrtInodeInb(int inodeid, char s[inodesz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//������+���+i�ڵ��
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.write(s, inodesz);
	iofile.close();
	return 1;
}

//д�ļ���
bool wrtBlock(int blockid, char s[blockSz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//�ض������ÿ����ʼλ��
	iofile.seekg(ios::beg + blockid * blockSz);
	//д
	iofile.write(s, blockSz);
	iofile.close();
	return 1;
}

//��Inode�ڵ�
Inode rdInode(int inodeid) {
	Inode res;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//������+���+i�ڵ��
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	iofile.read((char*)&res, sizeof(Inode));
	iofile.close();
	return res;
}

//�����ƶ�Inode
void rdInodeInb(int inodeid, char s[inodesz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//�ض������ÿ����ʼλ��
	int poi = blockSz + inodeid *inodesz;
	iofile.seekg(ios::beg + poi);
	//��
	iofile.read(s, inodesz);
	iofile.close();
	return;
}

//дĿ¼���ļ���
bool wrtBlock(int blockid, Menu menu) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//�ض������ÿ����ʼλ��
	iofile.seekg(ios::beg + blockid * blockSz);
	//д
	iofile.write((char*)&menu, sizeof(Menu));
	iofile.close();
	return 1;
}

//���ļ���
void rdBlock(int blockid, char s[blockSz]) {
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//�ض������ÿ����ʼλ��
	iofile.seekg(ios::beg + blockid * blockSz);
	//��
	iofile.read(s, blockSz);
	iofile.close();
	return ;
}

//���ļ����Ŀ¼
Menu rdBlockMenu(int blockid) {
	Menu menu;
	fstream iofile("#C.disk", ios::in | ios::out | ios::binary);
	//�ض������ÿ����ʼλ��
	iofile.seekg(ios::beg + blockid * blockSz);
	//��
	iofile.read((char*)&menu, sizeof(Menu));
	iofile.close();
	return menu;
}


//ע�⣡�ַ���������Ҫ��/0 ����Ϊ�ļ��鲻��Ҫ/0��
#endif