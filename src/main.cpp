#define _CRT_SECURE_NO_WARNINGS
#include"superblock.h"
#include"inode.h"
#include"diskRW.h"
#include"freeBlockManage.h"
#include"freeInodeManage.h"
#include"extraFun.h"
#include"logicManage.h"
#include"docTree.h"
#include<fstream>
#include<iostream>
#include<conio.h>
#include<iomanip>
#include<cstdio>
#include<iostream>
#include<sstream>
using namespace std;

//当前位置
int curInodeid;
Inode curInode;
Menu curMenu;
//储存当前路径
vector<string> route;
//用于文件拷贝
Inode cpInode;
string cpString;
string cpName;
//说明文件
string cmds[100] = {
	"ls", "chmod", "chown", "chgrp", "cd", "mkdir", "rmdir", "umask", "mv", "cp", "pst","vi","apd","rm", "ln", "cat",
	"passwd", "help", "exit"
};
string clrs[100] = {
	"显示本文件夹下所有信息 用法：ls",
	"改变文件对他人的权限,只有文件拥有着或管理员可用,有具体提示 用法：chmod <新权限编号>",
	"改变文件拥有者,只有文件拥有着或管理员可用 用法：chown <新拥有者名称>",
	"改变文件所属组,只有管理员可用 用法：chgrp <新组名>",
	"打开目录文件 用法：cd <当前目录下的目录名称>",
	"创建新目录文件 用法：mkdir <新目录名称>",
	"删除目录文件 注意该目录下的所有文件都会被删除 用法：rmdir <待删除的目录名称>",
	"改变初始权限 用法：umask <新初始目录权限>",
	"重命名文本文件或目录文件 用法：mv <旧名> <新名>",
	"复制文本文件到剪贴板 用法：cp <文件名>",
	"将剪贴板内的文件粘贴到当前目录 用法：pst",
	"将某个指定长度的字符串加到目标文件之后 用法：apd <目标文件名> <字符串长度>",
	"创建新文件 用法：vi <新文件名>",
	"删除文件 删除连接文件时不删除文件本身 用法：rm <文件名>",
	"创建连接（快捷方式）在起始目录中创建一个目的文本/目录的快捷方式 快捷方式以@开头 用法：ln <目的文本或目录> <起始目录>",
	"读/写文件 根据权限决定 用法： cat <文件名>",
	"修改当前用户的密码 用法：passwd <新密码>",
	"获得帮助 用法 help <all>|<指令名>",
	"安全退出 用法 exit"
};

//
//判断当前用户对该文件的权限
int isowner(CurUser curUser, Inode inode) {
	if (curUser.username == "admin") return 3;
	if (curUser.username == inode.onwer){
		if (inode.visMod[0][2]) return 3;
		else if (inode.visMod[0][1]) return 1;
		else if (inode.visMod[0][0]) return 0;
		else return -1;
	}
	else if (curUser.group == inode.group){
		if (inode.visMod[1][2]) return 2;
		else if (inode.visMod[1][1]) return 1;
		else if (inode.visMod[1][0]) return 0;
		else return -1;
	}
	else {
		if (inode.visMod[2][2]) return 2;
		else if (inode.visMod[2][1]) return 1;
		else if (inode.visMod[2][0]) return 0;
		else return -1;
	}
}

void input(char s[]) {
	string in;
	cin >> in;
	if (in.length() > stdls-1) {
		printf("过长字符串输入自动截断\n");
		for (int i = 0; i < stdls-1; i++) s[i] = in[i];
		s[stdls-1] = 0;
	}
	else {
		for (int i = 0; i < in.length(); i++) s[i] = in[i];
		s[in.length()] = 0;
	}
}

void inputPassword(char s[]) {
	string in;
	char ch = 0;
	while (ch = _getch()) {
		if (ch == '\n' || ch == '\r') break;
		if (ch == '\b') {
			in = in.substr(0, in.length() - 1);
		}
		else {
			printf("*");
			in += ch;
		}
	}
	if (in.length() > stdls-1) {
		printf("过长字符串输入自动截断\n");
		for (int i = 0; i < stdls-1; i++) s[i] = in[i];
		s[stdls-1] = 0;
	}
	else {
		for (int i = 0; i < in.length(); i++) s[i] = in[i];
		s[in.length()] = 0;
	}
	puts("");
}

void printRoute() {
	printf(">>>");
	for (int i = 0; i < route.size(); i++) {
		cout << route[i];
		if (i != (int)route.size() - 1) printf("/");
		else printf(">");
	}
}

void logicInit() {
	//初始化
	while (1) {
		printf("重置系统 = 0 继续开始 = 1\n");
		int fg; scanf("%d", &fg);
		if (fg == 0) {
			fresh();
			route.push_back("root");
			break;
		}
		else {
			Sprblk tmpSprblk = rdSpblk();
			if (tmpSprblk.blockTot != totBlock) {
				printf("系统还未初始化！请选择重置系统\n");
				system("pause");
				system("cls");
			}
			else {
				sprblk = tmpSprblk;
				route.push_back("root");
				break;
			}
		}
	}
}

void login_register(){
	//登陆/注册
	while (1) {
		printf("登陆 = 0 注册 = 1\n");
		int fg; scanf("%d", &fg);
		if (fg == 0) {
			char username[stdls], password[stdls];
			printf("请输入用户名：\n");
			input(username);
			printf("请输入密码：\n");
			inputPassword(password);
			bool loginSuccess = 0;
			for (int i = 0; i < 5; i++) {
				if (strcmp(sprblk.username[i], username) == 0 && strcmp(sprblk.password[i], password) == 0) {
					puts("登陆成功！");
					curUser.username = username;
					if (curUser.username == "admin") curUser.group = "admin";
					else curUser.group = "user";
					loginSuccess = 1;
					break;
				}
			}
			if (loginSuccess) break;
			else {
				puts("登陆失败！");
			}
		}
		else {
			char username[stdls], password[stdls], repassword[stdls];
			printf("请输入用户名：\n");
			input(username);
			printf("请输入密码：\n");
			inputPassword(password);
			printf("请重复输入密码\n");
			inputPassword(repassword);
			bool registerSuccess = 0;
			for (int i = 0; i < 5; i++) {
				if (sprblk.username[i][0] == 0){
					if (strcmp(password, repassword) == 0) {
						strcpy(sprblk.username[i], username);
						strcpy(sprblk.password[i], password);
						registerSuccess = 1;
						curUser.username = username;
						if (curUser.username == "admin") curUser.group = "admin";
						else curUser.group = "user";
						printf("注册成功！\n");
						break;
					}
					else {
						puts("两次密码不一致");
						break;
					}
				}
			}
			if (registerSuccess) break;
			else {
				printf("注册失败！\n");
			}
		}
	}
}

int serMenu(const char s[]) {
	for (int i = 0; i < curMenu.it; i++) {
		if (strcmp(s, curMenu.item[i].name) == 0) {
			return curMenu.item[i].addr;
		}
	}
	return -1;
}

bool noLink(int inodeid,int source) {
	Inode inode = rdInode(inodeid);
	if (inodeid!=source && inode.linkedMenu) return false;
	if (inode.type == 1) {
		Menu menu = readMenu(inode);
		for (int i = 2; i < menu.it; i++) {
			if (!noLink(menu.item[i].addr,source)) return false;
		}
	}
	return true;
}

void printHead() {
	cout << "[" << curUser.username << "@localhost ";
	if (route.size() == 1) cout << "~]";
	else cout << "/]";
	if (curUser.group == "admin") cout << "#";
	else cout << "$";
}

bool changeReadTime(int inodeid) {
	Inode inode = rdInode(inodeid);
	inode.lastRead = time(NULL);
	wrtInode(inodeid, inode);
	return 1;
}

bool changeWriteTime(int inodeid) {
	Inode inode = rdInode(inodeid);
	inode.lastWrite = time(NULL);
	wrtInode(inodeid, inode);
	return 1;
}

int main() {
	printf("Fake Unix\n(c) 2019 bennieHan\n");
	cout << sizeof(Sprblk) << endl;
	logicInit();
	login_register();

	curInodeid = sprblk.root;
	curInode = rdInode(curInodeid);
	curMenu = readMenu(curInode);

	while (1) {
		printHead();
		string cmd;
		cin >> cmd;
		if (cmd == "pwd") {
			printRoute();
			puts("");
		}
		//创建新文件 无需权限
		if (cmd == "vi") {
			char fname[stdls];
			input(fname);
			if (serMenu(fname)!=-1) {
				puts("该文件名已存在");
				continue;
			}
			//为新文件分配储存空间
			//输入文本
			string doc;
			puts("你可以在大小写字母写入及数字 非法输入视为输入结束");
			getchar();
			while (char ch = getchar()) {
				if (!(ch >= 'a' && ch <= 'z' || ch >= 'A'&& ch <= 'Z' || ch >= '0' && ch <= '9')) break;
				doc += ch;
			}
			int fid = iallocDoc(curUser,doc);
			//更新目录
			iaddMenu(curInodeid, Item(fname,fid));
			//更新当前目录
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		//创建文件夹 无需权限
		if (cmd == "mkdir") {
			char mnname[stdls];
			input(mnname);
			if (serMenu(mnname)!=-1) {
				puts("该文件名已存在");
				continue;
			}
			int mnid = iallocMenu(curInodeid, curUser);
			iaddMenu(curInodeid, Item(mnname, mnid));
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		//删除文件夹 需要删除权限2
		if (cmd == "rmdir") {
			char mnname[stdls];
			input(mnname);
			int toDel = serMenu(mnname);
			Inode toDelNode = rdInode(toDel);
			if (toDel == -1) {
				puts("找不到文件夹");
				continue;
			}
			if (!noLink(toDel,toDel)) {
				puts("该文件夹中有被链接的文件");
				continue;
			}
			if (isowner(curUser, toDelNode) < 2) {
				puts("权限不足");
				continue;
			}
			if (mnname[0] != '@') {
				if (toDelNode.linkedMenu) {
					puts("请先删除连接向该文件的连接");
					continue;
				}
				if (ifreeMenu(toDel, curUser)) {
					idelMenu(curInodeid, Item(mnname, 0));
					curInode = rdInode(curInodeid);
					curMenu = readMenu(curInode);
				}
			}
			else {
				toDelNode.linkedMenu--;
				idelMenu(curInodeid, Item(mnname, 0));
				wrtInode(toDel, toDelNode);
				curInode = rdInode(curInodeid);
				curMenu = readMenu(curInode);
			}
			
		}
		//移动到文件夹 需要读权限0
		if (cmd == "cd") {
			char to[12];
			input(to);
			if (strlen(to) == 2 && to[0] == '.' && to[1] == '.') {
				if (curInodeid == sprblk.root) {
					printf("当前在根节点\n");
					continue;
				}
				route.pop_back();
				curInodeid = curMenu.item[0].addr;
				changeReadTime(curInodeid);
				curInode = rdInode(curInodeid);
				curMenu = readMenu(curInode);
			}
			else if (strlen(to) == 1 && to[0] == '.') continue;
			else {
				int toMove = serMenu(to);
				if (toMove == -1) {
					puts("找不到文件夹");
					continue;
				}
				Inode nextInode = rdInode(toMove);
				if (nextInode.type == 0) {
					puts("这不是一个目录文件");
					continue;
				}
				if (isowner(curUser, nextInode) < 0) {
					puts("权限不足！");
					continue;
				}
				
				route.push_back(to);
				curInodeid = toMove;
				changeReadTime(curInodeid);
				curInode = nextInode;
				curInode.lastRead = time(NULL);
				curMenu = readMenu(curInode);

				vector<int> rt;
				int tmpInodeid = curInodeid;
				if (to[0] == '@') {
					while (tmpInodeid != -1) {
						rt.push_back(tmpInodeid);
						Inode inode = rdInode(tmpInodeid);
						Menu cmenu = readMenu(inode);
						tmpInodeid = cmenu.item[0].addr;
					}
					route.clear();
					route.push_back("root");
					int cid = sprblk.root;
					Inode inode = rdInode(cid);
					Menu menu = readMenu(inode);
					for (int i = rt.size() - 2; i >= 0; i--) {
						for (int j = 0; j < menu.it; j++) {
							if (menu.item[j].addr == rt[i]) {
								route.push_back(menu.item[j].name);
								cid = rt[i];
								inode = rdInode(cid);
								menu = readMenu(inode);
								break;
							}
						}
					}
				}
			}
		}
		//显示目录下文件 无需权限
		if (cmd == "ls") {
			changeReadTime(curInodeid);
			string chs; cin >> chs;
			if (chs == "-l") {
				for (int i = 2; i < curMenu.it; i++) {
					Inode tmp = rdInode(curMenu.item[i].addr);
					if (isowner(curUser, tmp) > -1) {
						if (tmp.type == 0) printf("-");
						else printf("d");
						for (int i = 0; i < 3; i++) {
							if (tmp.visMod[i][0] == 1) printf("r");
							else printf("-");
							if (tmp.visMod[i][1] == 1) printf("w");
							else printf("-");
							if (tmp.visMod[i][2] == 1) printf("x");
							else printf("-");
						}printf(". ");
						printf("%d ", tmp.linkedMenu);
						printf("%s ", tmp.onwer);
						printf("%s ", tmp.group);
						printf("%d ", tmp.docSize);
						char lastReadTime[64];
						strftime(lastReadTime, sizeof(lastReadTime), "%Y-%m-%d %H:%M:%S", localtime(&tmp.lastRead));
						printf("%s ", lastReadTime);
						printf("%s\n", curMenu.item[i].name);
					}
				}
			}
			else {
				for (int i = 2; i < curMenu.it; i++) {
					Inode tmp = rdInode(curMenu.item[i].addr);
					if (isowner(curUser, tmp) > -1) {
						cout << setw(stdls) << curMenu.item[i].name << " ";
						if (tmp.type == 0 && i >= 2) puts("文件");
						else puts("目录");
					}
				}
			}
			puts("-------------------END-------------------");
		}
		//读/写文件 需要权限0/1
		if (cmd == "cat") {
			char to[stdls];
			input(to);
			bool catok = 0;
			int toCat = serMenu(to);
			if (toCat == -1) {
				 puts("文件名输入错误");
				 continue;
			}

			Inode nextInode = rdInode(toCat);
			if (nextInode.type == 1) {
				puts("这不是一个文本文件");
				continue;
			}
			int rt;
			if ((rt = isowner(curUser, nextInode)) < 0) {
				puts("权限不足！");		
				continue;
			}
			string old = readDoc(nextInode);
			string in;
			changeReadTime(toCat);

			if (rt == 0) {
				puts("当前权限只能读文件：");
				cout << old << endl;
				system("pause");
				catok = 1;
			}
			else {
				puts("你可以在原文件续接大小写字母及数字 非法输入视为输入结束");
				cout << old;
				getchar();
				while (char ch = getchar()) {
					if (!(ch >= 'a' && ch <= 'z' || ch >= 'A'&& ch <= 'Z' || ch >= '0' && ch <= '9')) break;
					in += ch;
				}
				iappendDoc(toCat, in);
				changeWriteTime(toCat);
			}
		}
		if (cmd == "shwown") {
			char to[stdls] = { 0 };
			input(to);
			int roShow = serMenu(to);
			if (roShow == -1) {
				puts("文件不存在");
				continue;
			}
			Inode nextInode = rdInode(roShow);
			printf("类型：");
			if (nextInode.type == 0) puts("文本文件");
			else puts("目录文件");

			printf("拥有者：%s\n", nextInode.onwer);
			printf("所属组：%s\n", nextInode.group);
			char lastReadTime[64],lastWriteTime[64];
			strftime(lastReadTime, sizeof(lastReadTime), "%Y-%m-%d %H:%M:%S", localtime(&nextInode.lastRead));
			printf("最后访问时间：%s\n", lastReadTime);
			strftime(lastWriteTime, sizeof(lastWriteTime), "%Y-%m-%d %H:%M:%S", localtime(&nextInode.lastWrite));
			printf("最后修改时间：%s\n", lastWriteTime);
			changeReadTime(roShow);
		}
		//改变文件权限 需要最高权限3
		if (cmd == "chmod") {
			char to[stdls] = { 0 };
			input(to);
			int newVisMod = -1;
			cin >> newVisMod;

			int toChmod = serMenu(to);
			if (toChmod == -1) {
				puts("文件不存在");
				continue;
			}
			Inode nextInode = rdInode(toChmod);
			if (newVisMod >= 0 && newVisMod <= 777 && isowner(curUser, nextInode) == 3) {
				int own = newVisMod / 100;
				int grp = (newVisMod / 10) % 10;
				int oth = newVisMod % 10;
				nextInode.visMod[0][0] = (own >> 2) & 1;
				nextInode.visMod[0][1] = (own >> 1) & 1;
				nextInode.visMod[0][2] = (own)& 1;
				nextInode.visMod[1][0] = (grp >> 2) & 1;
				nextInode.visMod[1][1] = (grp >> 1) & 1;
				nextInode.visMod[1][2] = (grp)& 1;
				nextInode.visMod[2][0] = (oth >> 2) & 1;
				nextInode.visMod[2][1] = (oth >> 1) & 1;
				nextInode.visMod[2][2] = (oth)& 1;
				wrtInode(toChmod, nextInode);
				puts("权限修改成功");
				changeWriteTime(toChmod);
			}
			else {
				puts("权限不足");
			}
		}

		//改变文件拥有者
		if (cmd == "chown") {
			char to[stdls] = { 0 };
			input(to);
			char newUsername[stdls];
			while (1) {
				puts("修改该文件拥有者 输入新拥有者用户名：");
				input(newUsername);
				int fg = 0;
				for (int i = 0; i < 5; i++) {
					if (strcmp(sprblk.username[i], newUsername) == 0) {
						fg = 1;
					}
				}
				if (!fg) puts("该用户不存在");
				else break;
			}
			int toChown = serMenu(to);
			if (toChown == -1) {
				puts("文件不存在");
				continue;
			}
			Inode nextInode = rdInode(toChown);
			if (isowner(curUser, nextInode) == 3) {
				strcpy(nextInode.onwer, newUsername);
				wrtInode(toChown, nextInode);
				puts("拥有者修改成功");
				changeWriteTime(toChown);
			}
			else {
				puts("权限不足");
			}
		}

		//改变文件所属组
		if (cmd == "chgrp") {
			char to[stdls] = { 0 };
			input(to);
			char newgroup[stdls];
			while (1) {
				puts("修改该文件所属组 输入新所属组组名：");
				input(newgroup);
				if (strcmp(newgroup, "user") == 0) break;
				if (strcmp(newgroup, "admin") == 0) break;
				puts("该组不存在");
			}
			int toChgrp = serMenu(to);
			if (toChgrp == -1) {
				puts("文件不存在"); 
				continue;
			}
			Inode nextInode = rdInode(toChgrp);
			if (isowner(curUser, nextInode) == 3) {
				strcpy(nextInode.group, newgroup);
				wrtInode(toChgrp, nextInode);
				puts("所属组修改成功");
				changeWriteTime(toChgrp);
			}
			else {
				puts("权限不足");
			}
		}

		if (cmd == "mv") {
			char to[stdls] = { 0 };
			input(to);
			char newname[stdls];
			while (1) {
				puts("修改该文件文件名 输入新文件名：");
				input(newname);
				int fg = 0;
				for (int i = 0; i<curMenu.it; i++) {
					if (strcmp(curMenu.item[i].name, newname) == 0) {
						fg = 1;
					}
				}
				if (fg) puts("该用户名已存在");
				else break;
			}
			int ok = 0;
			for (int i = 2; i < curMenu.it; i++) {
				if (strcmp(to, curMenu.item[i].name) == 0) {
					Inode nextInode = rdInode(curMenu.item[i].addr);
					if (isowner(curUser, nextInode) == 3) {
						idelMenu(curInodeid, Item(curMenu.item[i].name, curMenu.item[i].addr));
						strcpy(curMenu.item[i].name, newname);
						iaddMenu(curInodeid, Item(curMenu.item[i].name, curMenu.item[i].addr));
						puts("文件名修改成功");
						changeWriteTime(curMenu.item[i].addr);
					}
					else {
						puts("权限不足");
					}
					ok = 1;
				}
			}
			if (!ok) puts("找不到文件");
		}
		if (cmd == "cp") {
			char origin[stdls];
			input(origin);
			int originInode = serMenu(origin);
			if (originInode == -1) {
				puts("找不到文件");
				continue;
			}

			cpInode = rdInode(originInode);
			if (cpInode.type == 1) {
				puts("不支持复制文件夹");
				continue;
			}
			if (isowner(curUser, cpInode) < 2) {
				puts("权限不足");
				continue;
			}

			cpString = readDoc(cpInode);
			cpName = origin;
			puts("已复制");
			changeWriteTime(originInode);
		}
		if (cmd == "pst") {
			if (cpName.length() == 0) {
				puts("剪贴板为空");
				continue;
			}
			char cpname[stdls];
			int l = 0;
			for (int i = 0; i < cpName.length(); i++) {
				cpname[i] = cpName[i];
				l++;
			}cpname[cpName.length()] = 0;

			int ck = serMenu(cpname);
			while (ck != -1 && l<stdls-1) {
				cpname[l++] = '*';
				cpname[l] = 0;
				ck = serMenu(cpname);;
			}
			if (l == stdls-1) {
				puts("文件名过长");
				continue;
			}
			cpname[l] = 0;
			
			int newInodeId = iallocDoc(curUser,cpString);
			//更新目录
			iaddMenu(curInodeid, Item(cpname, newInodeId));
			//更新当前目录
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		if (cmd == "ln") {
			char from[200],to[200];
			scanf("%s%s", from,to);
			int lf = strlen(from);
			int lt = strlen(to);
			if (curUser.group != "admin") {
				puts("管理员组才可执行本操作");
				continue;
			}
			//分解路径
			vector<string>route1;
			vector<string>route2;
			string res;
			for (int i = 0; i < lf; i++) {
				if (from[i] == '/') {
					route1.push_back(res);
					res = "";
				}
				else res += from[i];
			}route1.push_back(res);
			res = "";
			for (int i = 0; i < lt; i++) {
				if (to[i] == '/') {
					route2.push_back(res);
					res = "";
				}
				else res += to[i];
			}route2.push_back(res);
			//获取rt1的inode节点
			int nodeid = sprblk.root;
			int fg = 0;
			for (int i = 1; i < route1.size(); i++) {
				Inode inode = rdInode(nodeid);
				Menu cmenu = readMenu(inode);
				fg = 0;
				for (int j = 0; j < cmenu.it; j++) {
					if (cmenu.item[j].name == route1[i]) {
						nodeid = cmenu.item[j].addr;
						fg = 1;
					}
				}
				if (!fg) {
					puts("找不到对应文件");
					break;
				}
			}
			if (!fg && route1.size()>1) continue;
			fg = 0;
			//获取rt2的位置
			int des = sprblk.root;
			for (int i = 1; i < route2.size(); i++) {
				Inode inode = rdInode(des);
				Menu cmenu = readMenu(inode);
				fg = 0;
				for (int j = 0; j < cmenu.it; j++) {
					if (cmenu.item[j].name == route2[i]) {
						des = cmenu.item[j].addr;
						fg = 1;
						break;
					}
				}
				if (!fg) {
					puts("找不到对应文件");
					break;
				}
			}
			if (!fg && route2.size()>1) continue;

			Inode inode = rdInode(des);
			if (inode.type == 0) {
				puts("文本文件不能作为连接起点");
				continue;
			}

			char name[stdls];
			Menu cmenu = readMenu(inode);
			if ((int)route1[route1.size() - 1].length() >= 10) {
				puts("文件名过长 创建连接失败");
				continue;
			}
			name[0] = '@';
			for (int i = 0; i<min((int)route1[route1.size() - 1].length(), stdls); i++) {
				name[i+1] = route1[route1.size() - 1][i];
			}
			name[route1[route1.size()-1].size()+1] = 0;

			cmenu.add(name, nodeid);

			Inode frominode = rdInode(nodeid);
			frominode.linkedMenu++;
			wrtInode(nodeid, frominode);

			delDoc(inode);
			saveDoc(inode, menuToString(cmenu));
			wrtInode(des, inode);

			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		if (cmd == "apd") {
			char to[stdls];
			input(to);
			int toApd = serMenu(to);

			string tmpsz;
			cin >> tmpsz;
			stringstream ss(tmpsz);
			int sz; ss >> sz;
			if (sz < 0) {
				puts("非法文件长度");
				continue;
			}
			if (toApd == -1) {
				puts("找不到文件");
				continue;
			}
			Inode inode = rdInode(toApd);
			if (isowner(curUser, inode) < 1) {
				puts("权限不足");
				continue;
			}
			if (inode.type != 0) {
				puts("不是文本文件");
				continue;
			}
			changeReadTime(toApd);
			changeWriteTime(toApd);
			string newDoc;
			for (int i = 0; i < sz; i++) newDoc += 'a';
			iappendDoc(toApd, newDoc);
		}
		if (cmd == "rm") {
			char to[stdls];
			input(to);
			int toRmove = serMenu(to);
			if (toRmove == -1) {
				puts("找不到文件");
				continue;
			}
			Inode inode = rdInode(toRmove);
			if (isowner(curUser, inode) < 2) {
				puts("权限不足");
				continue;
			}

			if (inode.type != 0) {
				puts("不是文本文件，请使用rmdir命令删除文件夹");
				continue;
			}
			
			if (to[0] != '@') {
				if (inode.linkedMenu) {
					puts("请先删除连接向该文件的连接");
					continue;
				}
				ifreeDoc(toRmove);
			}
			else {
				inode.linkedMenu--;
				wrtInode(toRmove, inode);
			}
			idelMenu(curInodeid, Item(to, toRmove));
			curMenu.remove(to);
			delDoc(curInode);
			saveDoc(curInode, menuToString(curMenu));
			wrtInode(curInodeid, curInode);
		}
		if (cmd == "passwd") {
			char to[stdls];
			inputPassword(to);
			for (int i = 0; i < 5; i++) {
				if (sprblk.username[i] == curUser.username) {
					strcpy(sprblk.password[i], to);
					break;
				}
			}
		}
		if (cmd == "help") {
			string cs; cin >> cs;
			if (cs == "all") {
				for (int i = 0; i < 18; i++) {
					cout << setw(stdls) << cmds[i] << " " << clrs[i] << endl << endl;
				}
			}
			else {
				for (int i = 0; i < 18; i++) {
					if (cmds[i] == cs)
						cout << setw(stdls) << cmds[i] << " " << clrs[i] << endl;
				}
			}
		}
		if (cmd == "umask") {
			int newUmask;
			cin >> newUmask;
			if (curUser.group != "admin") {
				puts("必须是管理员权限");
				continue;
			}
			if (newUmask >= 0 && newUmask <= 777) {
				int own = newUmask / 100;
				int grp = (newUmask / 10) % 10;
				int oth = newUmask % 10;
				sprblk.umask[0][0] = (own >> 2) & 1;
				sprblk.umask[0][1] = (own >> 1) & 1;
				sprblk.umask[0][2] = (own) & 1;
				sprblk.umask[1][0] = (grp >> 2) & 1;
				sprblk.umask[1][1] = (grp >> 1) & 1;
				sprblk.umask[1][2] = (grp)& 1;
				sprblk.umask[2][0] = (oth >> 2) & 1;
				sprblk.umask[2][1] = (oth >> 1) & 1;
				sprblk.umask[2][2] = (oth)& 1;
			}
			else {
				puts("新默认权限非法");
			}
		}
		if (cmd == "exit") {
			break;
		}
	}
	getDestroy();
} 