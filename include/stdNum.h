#ifndef STDNUM
#define STDNUM

//����Ŀ640+��60/4��+1 = 656��
const int totBlock = 656;
const int inodeNum = 60; //i�ڵ�����
const int blockNum = 640; //���ļ����̿���Ŀ

//ջ�Ĵ�С
const int blockStaSize = 32;
const int inodeStaSize = 20;
//ÿ���С���ֽڣ�
const int blockSz = 512;
//ÿ��inode��ռλ (inodeʵ�ʴ�СΪ72 ����˸�������ʱ��ı��� ����һЩ����)
const int inodesz = 128;
//ÿ���м���Inode
const int B_I = 4;
//ÿ��������С <512/4 = 128
const int indexsz = 16;
//�ַ����������� ʵ������11��Ϊ\0
const int stdls = 12;

#endif