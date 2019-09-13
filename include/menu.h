#ifndef MENU
#define MENU
#include<cstring>
#include"inode.h"
#include"extraFun.h"
using namespace std;

struct Item {
	char name[stdls];//�ļ���
	int addr; //�ļ�i�ڵ��ַ
	Item(){
		memset(name, 0, sizeof(name));
		addr = 0;
	}
	Item(char s[],int a) {
		strcpy(name, s);
		addr = a;
	}
};

struct Menu {
	int it;
	Item item[24];
	Menu(){
		it = 0;
		memset(item, 0, sizeof(item));
	}
	Menu(int inodeId, int pa) {
		it = 2;
		memset(item, 0, sizeof(item));
		//0��Ԫ���ǰְ�
		item[0].name[0] = '.';
		item[0].name[1] = '.';
		item[0].addr = pa;
		//1��Ԫ�����Լ�
		item[1].name[0] = '.';
		item[1].addr = inodeId;
	}
	//��Ŀ¼�����Ԫ��
	bool add(char name[stdls], int addr) {
		for (int i = 0; i < it; i++) {
			if (strcmp(item[i].name, name) == 0) return 0;
		}
		for (int i = 0; i < stdls; i++) item[it].name[i] = name[i];
		item[it].addr = addr;
		it++;
		return 1;
	}
	//��Ŀ¼ɾ��,����addr
	int remove(char name[stdls]) {
		int res = -1;
		for (int i = 0; i < it; i++) {
			if (strcmp(name, item[i].name) == 0) {
				res = item[i].addr;
				for (int j = i; j < it-1; j++) {
					item[j] = item[j + 1];
				}
				it--;
				return res;
			}
		}
		return res;
	}
};

#endif