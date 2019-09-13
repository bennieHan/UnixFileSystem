#ifndef INODE
#define INODE
#include<time.h>
#include<string>
#include"stdNum.h"
#include"superblock.h"
using namespace std;

//�û� ���ڼ�¼��ǰ�û�
struct CurUser {
	string username; //��ǰ�û��û���
	string group; //��ǰ�û�������
	CurUser(){}
	CurUser(string _username, string _group) {
		username = _username;
		group = _group;
	}
}curUser;

//����64B
struct Inode {
	bool type; //0�ļ� 1Ŀ¼
	bool visMod[3][3]; //����ģʽ �ֱ�Ϊ ӵ���� ͬ�� �����û��Ķ�дִ��
	time_t lastRead;
	time_t lastWrite;
	int docSize; //�ļ���С ��λ���ֽ�
	short dirStr[4]; //ֱ�ӿ��
	short lv1; //һ����ӵ�ַ
	short lv2; //һ����ӵ�ַ
	short linkedMenu; //�ı��ļ���������ʹ��
	char onwer[stdls]; //ӵ����
	char group[stdls]; //������
	Inode(bool type = 0, int docSize = 0, CurUser curUser = CurUser("", ""))
		:type(type),docSize(docSize){
		for (int i = 0; i < 4; i++) dirStr[i] = -1;
		lv1 = -1; lv2 = -1;
		linkedMenu = 0;
		lastRead = time(NULL);
		lastWrite = time(NULL);
		for (int i = 0; i < curUser.username.length(); i++) {
			onwer[i] = curUser.username[i];
		}onwer[curUser.username.length()] = 0;
		for (int i = 0; i < curUser.group.length(); i++) {
			group[i] = curUser.group[i];
		}group[curUser.group.length()] = 0;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				visMod[i][j] = sprblk.umask[i][j] ^ 1;
			}
		}
		linkedMenu = 0;
	}
};


#endif