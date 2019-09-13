#ifndef INODE
#define INODE
#include<time.h>
#include<string>
#include"stdNum.h"
#include"superblock.h"
using namespace std;

//用户 用于记录当前用户
struct CurUser {
	string username; //当前用户用户名
	string group; //当前用户所属组
	CurUser(){}
	CurUser(string _username, string _group) {
		username = _username;
		group = _group;
	}
}curUser;

//正好64B
struct Inode {
	bool type; //0文件 1目录
	bool visMod[3][3]; //访问模式 分别为 拥有者 同组 其他用户的读写执行
	time_t lastRead;
	time_t lastWrite;
	int docSize; //文件大小 单位：字节
	short dirStr[4]; //直接块号
	short lv1; //一级间接地址
	short lv2; //一级间接地址
	short linkedMenu; //文本文件可以连接使用
	char onwer[stdls]; //拥有者
	char group[stdls]; //所属组
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