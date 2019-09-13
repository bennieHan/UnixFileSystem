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

//��ǰλ��
int curInodeid;
Inode curInode;
Menu curMenu;
//���浱ǰ·��
vector<string> route;
//�����ļ�����
Inode cpInode;
string cpString;
string cpName;
//˵���ļ�
string cmds[100] = {
	"ls", "chmod", "chown", "chgrp", "cd", "mkdir", "rmdir", "umask", "mv", "cp", "pst","vi","apd","rm", "ln", "cat",
	"passwd", "help", "exit"
};
string clrs[100] = {
	"��ʾ���ļ�����������Ϣ �÷���ls",
	"�ı��ļ������˵�Ȩ��,ֻ���ļ�ӵ���Ż����Ա����,�о�����ʾ �÷���chmod <��Ȩ�ޱ��>",
	"�ı��ļ�ӵ����,ֻ���ļ�ӵ���Ż����Ա���� �÷���chown <��ӵ��������>",
	"�ı��ļ�������,ֻ�й���Ա���� �÷���chgrp <������>",
	"��Ŀ¼�ļ� �÷���cd <��ǰĿ¼�µ�Ŀ¼����>",
	"������Ŀ¼�ļ� �÷���mkdir <��Ŀ¼����>",
	"ɾ��Ŀ¼�ļ� ע���Ŀ¼�µ������ļ����ᱻɾ�� �÷���rmdir <��ɾ����Ŀ¼����>",
	"�ı��ʼȨ�� �÷���umask <�³�ʼĿ¼Ȩ��>",
	"�������ı��ļ���Ŀ¼�ļ� �÷���mv <����> <����>",
	"�����ı��ļ��������� �÷���cp <�ļ���>",
	"���������ڵ��ļ�ճ������ǰĿ¼ �÷���pst",
	"��ĳ��ָ�����ȵ��ַ����ӵ�Ŀ���ļ�֮�� �÷���apd <Ŀ���ļ���> <�ַ�������>",
	"�������ļ� �÷���vi <���ļ���>",
	"ɾ���ļ� ɾ�������ļ�ʱ��ɾ���ļ����� �÷���rm <�ļ���>",
	"�������ӣ���ݷ�ʽ������ʼĿ¼�д���һ��Ŀ���ı�/Ŀ¼�Ŀ�ݷ�ʽ ��ݷ�ʽ��@��ͷ �÷���ln <Ŀ���ı���Ŀ¼> <��ʼĿ¼>",
	"��/д�ļ� ����Ȩ�޾��� �÷��� cat <�ļ���>",
	"�޸ĵ�ǰ�û������� �÷���passwd <������>",
	"��ð��� �÷� help <all>|<ָ����>",
	"��ȫ�˳� �÷� exit"
};

//
//�жϵ�ǰ�û��Ը��ļ���Ȩ��
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
		printf("�����ַ��������Զ��ض�\n");
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
		printf("�����ַ��������Զ��ض�\n");
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
	//��ʼ��
	while (1) {
		printf("����ϵͳ = 0 ������ʼ = 1\n");
		int fg; scanf("%d", &fg);
		if (fg == 0) {
			fresh();
			route.push_back("root");
			break;
		}
		else {
			Sprblk tmpSprblk = rdSpblk();
			if (tmpSprblk.blockTot != totBlock) {
				printf("ϵͳ��δ��ʼ������ѡ������ϵͳ\n");
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
	//��½/ע��
	while (1) {
		printf("��½ = 0 ע�� = 1\n");
		int fg; scanf("%d", &fg);
		if (fg == 0) {
			char username[stdls], password[stdls];
			printf("�������û�����\n");
			input(username);
			printf("���������룺\n");
			inputPassword(password);
			bool loginSuccess = 0;
			for (int i = 0; i < 5; i++) {
				if (strcmp(sprblk.username[i], username) == 0 && strcmp(sprblk.password[i], password) == 0) {
					puts("��½�ɹ���");
					curUser.username = username;
					if (curUser.username == "admin") curUser.group = "admin";
					else curUser.group = "user";
					loginSuccess = 1;
					break;
				}
			}
			if (loginSuccess) break;
			else {
				puts("��½ʧ�ܣ�");
			}
		}
		else {
			char username[stdls], password[stdls], repassword[stdls];
			printf("�������û�����\n");
			input(username);
			printf("���������룺\n");
			inputPassword(password);
			printf("���ظ���������\n");
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
						printf("ע��ɹ���\n");
						break;
					}
					else {
						puts("�������벻һ��");
						break;
					}
				}
			}
			if (registerSuccess) break;
			else {
				printf("ע��ʧ�ܣ�\n");
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
		//�������ļ� ����Ȩ��
		if (cmd == "vi") {
			char fname[stdls];
			input(fname);
			if (serMenu(fname)!=-1) {
				puts("���ļ����Ѵ���");
				continue;
			}
			//Ϊ���ļ����䴢��ռ�
			//�����ı�
			string doc;
			puts("������ڴ�Сд��ĸд�뼰���� �Ƿ�������Ϊ�������");
			getchar();
			while (char ch = getchar()) {
				if (!(ch >= 'a' && ch <= 'z' || ch >= 'A'&& ch <= 'Z' || ch >= '0' && ch <= '9')) break;
				doc += ch;
			}
			int fid = iallocDoc(curUser,doc);
			//����Ŀ¼
			iaddMenu(curInodeid, Item(fname,fid));
			//���µ�ǰĿ¼
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		//�����ļ��� ����Ȩ��
		if (cmd == "mkdir") {
			char mnname[stdls];
			input(mnname);
			if (serMenu(mnname)!=-1) {
				puts("���ļ����Ѵ���");
				continue;
			}
			int mnid = iallocMenu(curInodeid, curUser);
			iaddMenu(curInodeid, Item(mnname, mnid));
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		//ɾ���ļ��� ��Ҫɾ��Ȩ��2
		if (cmd == "rmdir") {
			char mnname[stdls];
			input(mnname);
			int toDel = serMenu(mnname);
			Inode toDelNode = rdInode(toDel);
			if (toDel == -1) {
				puts("�Ҳ����ļ���");
				continue;
			}
			if (!noLink(toDel,toDel)) {
				puts("���ļ������б����ӵ��ļ�");
				continue;
			}
			if (isowner(curUser, toDelNode) < 2) {
				puts("Ȩ�޲���");
				continue;
			}
			if (mnname[0] != '@') {
				if (toDelNode.linkedMenu) {
					puts("����ɾ����������ļ�������");
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
		//�ƶ����ļ��� ��Ҫ��Ȩ��0
		if (cmd == "cd") {
			char to[12];
			input(to);
			if (strlen(to) == 2 && to[0] == '.' && to[1] == '.') {
				if (curInodeid == sprblk.root) {
					printf("��ǰ�ڸ��ڵ�\n");
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
					puts("�Ҳ����ļ���");
					continue;
				}
				Inode nextInode = rdInode(toMove);
				if (nextInode.type == 0) {
					puts("�ⲻ��һ��Ŀ¼�ļ�");
					continue;
				}
				if (isowner(curUser, nextInode) < 0) {
					puts("Ȩ�޲��㣡");
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
		//��ʾĿ¼���ļ� ����Ȩ��
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
						if (tmp.type == 0 && i >= 2) puts("�ļ�");
						else puts("Ŀ¼");
					}
				}
			}
			puts("-------------------END-------------------");
		}
		//��/д�ļ� ��ҪȨ��0/1
		if (cmd == "cat") {
			char to[stdls];
			input(to);
			bool catok = 0;
			int toCat = serMenu(to);
			if (toCat == -1) {
				 puts("�ļ����������");
				 continue;
			}

			Inode nextInode = rdInode(toCat);
			if (nextInode.type == 1) {
				puts("�ⲻ��һ���ı��ļ�");
				continue;
			}
			int rt;
			if ((rt = isowner(curUser, nextInode)) < 0) {
				puts("Ȩ�޲��㣡");		
				continue;
			}
			string old = readDoc(nextInode);
			string in;
			changeReadTime(toCat);

			if (rt == 0) {
				puts("��ǰȨ��ֻ�ܶ��ļ���");
				cout << old << endl;
				system("pause");
				catok = 1;
			}
			else {
				puts("�������ԭ�ļ����Ӵ�Сд��ĸ������ �Ƿ�������Ϊ�������");
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
				puts("�ļ�������");
				continue;
			}
			Inode nextInode = rdInode(roShow);
			printf("���ͣ�");
			if (nextInode.type == 0) puts("�ı��ļ�");
			else puts("Ŀ¼�ļ�");

			printf("ӵ���ߣ�%s\n", nextInode.onwer);
			printf("�����飺%s\n", nextInode.group);
			char lastReadTime[64],lastWriteTime[64];
			strftime(lastReadTime, sizeof(lastReadTime), "%Y-%m-%d %H:%M:%S", localtime(&nextInode.lastRead));
			printf("������ʱ�䣺%s\n", lastReadTime);
			strftime(lastWriteTime, sizeof(lastWriteTime), "%Y-%m-%d %H:%M:%S", localtime(&nextInode.lastWrite));
			printf("����޸�ʱ�䣺%s\n", lastWriteTime);
			changeReadTime(roShow);
		}
		//�ı��ļ�Ȩ�� ��Ҫ���Ȩ��3
		if (cmd == "chmod") {
			char to[stdls] = { 0 };
			input(to);
			int newVisMod = -1;
			cin >> newVisMod;

			int toChmod = serMenu(to);
			if (toChmod == -1) {
				puts("�ļ�������");
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
				puts("Ȩ���޸ĳɹ�");
				changeWriteTime(toChmod);
			}
			else {
				puts("Ȩ�޲���");
			}
		}

		//�ı��ļ�ӵ����
		if (cmd == "chown") {
			char to[stdls] = { 0 };
			input(to);
			char newUsername[stdls];
			while (1) {
				puts("�޸ĸ��ļ�ӵ���� ������ӵ�����û�����");
				input(newUsername);
				int fg = 0;
				for (int i = 0; i < 5; i++) {
					if (strcmp(sprblk.username[i], newUsername) == 0) {
						fg = 1;
					}
				}
				if (!fg) puts("���û�������");
				else break;
			}
			int toChown = serMenu(to);
			if (toChown == -1) {
				puts("�ļ�������");
				continue;
			}
			Inode nextInode = rdInode(toChown);
			if (isowner(curUser, nextInode) == 3) {
				strcpy(nextInode.onwer, newUsername);
				wrtInode(toChown, nextInode);
				puts("ӵ�����޸ĳɹ�");
				changeWriteTime(toChown);
			}
			else {
				puts("Ȩ�޲���");
			}
		}

		//�ı��ļ�������
		if (cmd == "chgrp") {
			char to[stdls] = { 0 };
			input(to);
			char newgroup[stdls];
			while (1) {
				puts("�޸ĸ��ļ������� ������������������");
				input(newgroup);
				if (strcmp(newgroup, "user") == 0) break;
				if (strcmp(newgroup, "admin") == 0) break;
				puts("���鲻����");
			}
			int toChgrp = serMenu(to);
			if (toChgrp == -1) {
				puts("�ļ�������"); 
				continue;
			}
			Inode nextInode = rdInode(toChgrp);
			if (isowner(curUser, nextInode) == 3) {
				strcpy(nextInode.group, newgroup);
				wrtInode(toChgrp, nextInode);
				puts("�������޸ĳɹ�");
				changeWriteTime(toChgrp);
			}
			else {
				puts("Ȩ�޲���");
			}
		}

		if (cmd == "mv") {
			char to[stdls] = { 0 };
			input(to);
			char newname[stdls];
			while (1) {
				puts("�޸ĸ��ļ��ļ��� �������ļ�����");
				input(newname);
				int fg = 0;
				for (int i = 0; i<curMenu.it; i++) {
					if (strcmp(curMenu.item[i].name, newname) == 0) {
						fg = 1;
					}
				}
				if (fg) puts("���û����Ѵ���");
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
						puts("�ļ����޸ĳɹ�");
						changeWriteTime(curMenu.item[i].addr);
					}
					else {
						puts("Ȩ�޲���");
					}
					ok = 1;
				}
			}
			if (!ok) puts("�Ҳ����ļ�");
		}
		if (cmd == "cp") {
			char origin[stdls];
			input(origin);
			int originInode = serMenu(origin);
			if (originInode == -1) {
				puts("�Ҳ����ļ�");
				continue;
			}

			cpInode = rdInode(originInode);
			if (cpInode.type == 1) {
				puts("��֧�ָ����ļ���");
				continue;
			}
			if (isowner(curUser, cpInode) < 2) {
				puts("Ȩ�޲���");
				continue;
			}

			cpString = readDoc(cpInode);
			cpName = origin;
			puts("�Ѹ���");
			changeWriteTime(originInode);
		}
		if (cmd == "pst") {
			if (cpName.length() == 0) {
				puts("������Ϊ��");
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
				puts("�ļ�������");
				continue;
			}
			cpname[l] = 0;
			
			int newInodeId = iallocDoc(curUser,cpString);
			//����Ŀ¼
			iaddMenu(curInodeid, Item(cpname, newInodeId));
			//���µ�ǰĿ¼
			curInode = rdInode(curInodeid);
			curMenu = readMenu(curInode);
		}
		if (cmd == "ln") {
			char from[200],to[200];
			scanf("%s%s", from,to);
			int lf = strlen(from);
			int lt = strlen(to);
			if (curUser.group != "admin") {
				puts("����Ա��ſ�ִ�б�����");
				continue;
			}
			//�ֽ�·��
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
			//��ȡrt1��inode�ڵ�
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
					puts("�Ҳ�����Ӧ�ļ�");
					break;
				}
			}
			if (!fg && route1.size()>1) continue;
			fg = 0;
			//��ȡrt2��λ��
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
					puts("�Ҳ�����Ӧ�ļ�");
					break;
				}
			}
			if (!fg && route2.size()>1) continue;

			Inode inode = rdInode(des);
			if (inode.type == 0) {
				puts("�ı��ļ�������Ϊ�������");
				continue;
			}

			char name[stdls];
			Menu cmenu = readMenu(inode);
			if ((int)route1[route1.size() - 1].length() >= 10) {
				puts("�ļ������� ��������ʧ��");
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
				puts("�Ƿ��ļ�����");
				continue;
			}
			if (toApd == -1) {
				puts("�Ҳ����ļ�");
				continue;
			}
			Inode inode = rdInode(toApd);
			if (isowner(curUser, inode) < 1) {
				puts("Ȩ�޲���");
				continue;
			}
			if (inode.type != 0) {
				puts("�����ı��ļ�");
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
				puts("�Ҳ����ļ�");
				continue;
			}
			Inode inode = rdInode(toRmove);
			if (isowner(curUser, inode) < 2) {
				puts("Ȩ�޲���");
				continue;
			}

			if (inode.type != 0) {
				puts("�����ı��ļ�����ʹ��rmdir����ɾ���ļ���");
				continue;
			}
			
			if (to[0] != '@') {
				if (inode.linkedMenu) {
					puts("����ɾ����������ļ�������");
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
				puts("�����ǹ���ԱȨ��");
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
				puts("��Ĭ��Ȩ�޷Ƿ�");
			}
		}
		if (cmd == "exit") {
			break;
		}
	}
	getDestroy();
} 