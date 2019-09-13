#ifndef DOCTREE
#define DOCTREE
#include<vector>
#include<string>
#include<algorithm>
#include"superblock.h"
#include"inode.h"
#include"extraFun.h"
#include"diskRW.h"
#include"menu.h"
#include"freeBlockManage.h"
#include"freeInodeManage.h"
using namespace std;


//向节点写文本
bool saveDoc(Inode &inode, string str)
{
	//将doc截断成512B的小段
	vector<string> vec;
	int ls = str.length();
	string cut;
	for (int i = 0; i < ls; i++) {
		if (i && i % blockSz == 0) {
			vec.push_back(cut);
			cut = "";
		}
		cut += str[i];
	}
	if (cut.length()) vec.push_back(cut);
	if (vec.size() <= 4 && sprblk.freeBlockNum >= vec.size());
	else if (vec.size() >= 5 && vec.size() <= 20 && sprblk.freeBlockNum >= vec.size() + 1);
	else if (vec.size() >= 21 && sprblk.freeBlockNum >= vec.size() + 2 + ((int)vec.size() - 21) / indexsz + 1); 
	else return 0;
	int it = 0;
	//写直接节点
	for (it = 0; it < min(4, (int)vec.size()); it++) {
		int bnum = getBlock();
		if (bnum == -1) {
			return 0;
		}
		else {
			inode.dirStr[it] = bnum;
		}
		char sin[blockSz] = { 0 };
		for (int j = 0; j < vec[it].size(); j++) sin[j] = vec[it][j];
		wrtBlock(inode.dirStr[it], sin);
	}
	//写一级索引
	if (vec.size() > 4) {
		//得到一个块作为一级索引
		inode.lv1 = getBlock();
		if (inode.lv1 == -1) return 0;
		short lv1List[blockSz/2];
		memset(lv1List, -1, sizeof(lv1List));
		for (int i = 0; i < indexsz && it<vec.size(); i++, it++) {
			//一级索引的每一项指向一个存放文件的块
			lv1List[i] = getBlock();
			if (lv1List[i] == -1) return 0;

			char sin[blockSz] = { 0 };
			for (int j = 0; j < vec[it].size(); j++) sin[j] = vec[it][j];
			wrtBlock(lv1List[i], sin);
		}
		wrtBlock(inode.lv1, (char*)lv1List);
	}
	//写二级索引
	if (vec.size() > 4+indexsz) {
		inode.lv2 = getBlock();
		if (inode.lv2 == -1) return 0;

		short lv2List[blockSz/2];
		memset(lv2List, -1, sizeof(lv2List));
		int fg = 0;
		for (int i = 0; i < indexsz && it<vec.size(); i++) {
			lv2List[i] = getBlock();
			if (lv2List[i] == -1) return 0;

			short lv1List[blockSz/2];
			memset(lv1List, -1, sizeof(lv1List));
			for (int j = 0; j < indexsz && it<vec.size(); j++, it++) {
				lv1List[j] = getBlock();
				if (lv1List[j] == -1) return 0;

				char sin[blockSz] = { 0 };
				for (int k = 0; k < vec[it].size(); k++) sin[k] = vec[it][k];
				wrtBlock(lv1List[j], sin);
			}
			wrtBlock(lv2List[i], (char*)lv1List);
		}
		wrtBlock(inode.lv2, (char*)lv2List);
	}
	return 1;
}

//向节点读文本
string readDoc(Inode &inode)
{
	string res;
	//int nextBlock = inode.dirStr[0];
	//读直接地址
	for (int i = 0; i < 4; i++) {
		if (inode.dirStr[i] == -1) return res;
		char sout[blockSz] = { 0 };
		rdBlock(inode.dirStr[i], sout);
		for (int i = 0; i < blockSz; i++) {
			if (sout[i] == 0) return res;
			res += sout[i];
		}
	}
	//读一级间址
	short lv1List[blockSz/2] = { 0 };
	rdBlock(inode.lv1, (char*)lv1List);
	for (int i = 0; i < indexsz; i++) {
		if (lv1List[i] == -1) return res;
		char sout[blockSz] = { 0 };
		rdBlock(lv1List[i], sout);
		for (int i = 0; i < blockSz; i++) {
			if (sout[i] == 0) return res;
			res += sout[i];
		}
	}
	//读二级间址
	short lv2List[blockSz] = { 0 };
	rdBlock(inode.lv2, (char*)lv2List);
	for (int i = 0; i < indexsz; i++) {
		if (lv2List[i] == -1) return res;
		short lv1List[blockSz/2] = { 0 };
		rdBlock(lv2List[i], (char*)lv1List);
		for (int j = 0; j < indexsz; j++) {
			if (lv1List[j] == -1) return res;
			char sout[blockSz] = { 0 };
			rdBlock(lv1List[j], sout);
			for (int i = 0; i < blockSz; i++) {
				if (sout[i] == 0) return res;
				res += sout[i];
			}
		}
	}
	string rres;
	for (int i = 0; i < res.size(); i++) {
		if (res[i] == 0) break;
		rres += res[i];
	}
	return rres;
}

//删除节点的文本
bool delDoc(Inode &inode)
{
	int nextBlock = inode.dirStr[0];
	//删除直接地址
	for (int i = 0; i < 4; i++) {
		if (inode.dirStr[i] == -1) return 1;
		freeBlock(inode.dirStr[i]);
		inode.dirStr[i] = -1;
	}
	//删除一级间址
	if (inode.lv1 != -1) {
		short lv1List[blockSz/2] = { 0 };
		rdBlock(inode.lv1, (char*)lv1List);
		for (int i = 0; i < indexsz; i++) {
			if (lv1List[i] == -1) continue;
			freeBlock(lv1List[i]);
			lv1List[i] = -1;
		}
		freeBlock(inode.lv1);
		inode.lv1 = -1;
	}
	//删除二级间址
	if (inode.lv2 != -1) {
		short lv2List[blockSz/2] = { 0 };
		rdBlock(inode.lv2, (char*)lv2List);
		for (int i = 0; i < indexsz; i++) {
			if (lv2List[i] == -1) continue;
			short lv1List[blockSz/2] = { 0 };
			rdBlock(lv2List[i], (char*)lv1List);
			for (int j = 0; j < indexsz; j++) {
				if (lv1List[j] == -1) continue;
				freeBlock(lv1List[j]);
				lv1List[j] = -1;
			}
			freeBlock(lv2List[i]);
			lv2List[i] = -1;
		}
		freeBlock(inode.lv2);
		inode.lv2 = -1;
	}
	return 1;
}

//向节点读目录
Menu readMenu(Inode inode)
{
	Menu menu;
	Inode tmpnode = inode;
	for (int i = 0; i < 4; i++) {
		if (tmpnode.dirStr[i] == -1) return menu;
		menu = rdBlockMenu(tmpnode.dirStr[i]);
	}
	return menu;
}

string menuToString(Menu menu) {
	string res;
	for (int i = 0; i < (int)sizeof(Menu); i++) {
		res += ((char*)&menu)[i];
	}
	return res;
}

//新建文本
int iallocDoc(CurUser curUser,string doc) {
	//新建
	int inodeId = getInode();
	if (inodeId == -1) {
		puts("inode节点不足");
		return -1;
	}
	Inode inode(0, doc.size(), curUser);
	//储存
	if (saveDoc(inode, doc)) {
		wrtInode(inodeId, inode);
		return inodeId;
	}
	else {
		puts("系统空间不足");
		return -1;
	}
}

//新建目录
int iallocMenu(int curInodeId,CurUser curUser) {
	int inodeId = getInode();
	if (inodeId == -1) {
		puts("inode节点不足");
		return -1;
	}
	Menu menu(inodeId, curInodeId);
	Inode inode(1, sizeof(menu), curUser);
	if (saveDoc(inode, menuToString(menu))) {
		wrtInode(inodeId, inode);
		return inodeId;
	}
	else {
		puts("系统空间不足");
		return -1;
	}
}

//删除文本
bool ifreeDoc(int inodeId) {
	Inode inode = rdInode(inodeId);
	delDoc(inode);
	freeInode(inodeId);
	return 1;
}

//删除目录，需要把目录下的文件全部删除
bool ifreeMenu(int inodeId,CurUser curUser) {
	Inode inode = rdInode(inodeId);
	if (inode.type == 0) {
		puts("该文件不是目录文件！");
		return 0;
	}
	Menu menu = readMenu(inode);
	for (int i = 2; i < menu.it; i++) {
		int nid = menu.item[i].addr;
		Inode nnode = rdInode(nid);
		if (nnode.type == 0) {
			//删除内容
			ifreeDoc(nid);
			//释放节点
			freeInode(nid);
		}
		else {
			ifreeMenu(nid,CurUser("admin","admin"));
			freeInode(nid);
		}
	}
	delDoc(inode);
	freeInode(inodeId);
	return 1;
}

bool iappendDoc(int inodeId, string newdoc) {
	Inode inode = rdInode(inodeId);
	string olddoc = readDoc(inode);
	string newone = olddoc + newdoc;
	//1-23 更新明确了是超过单个文件大小限制 还是超过系统总大小限制
	if (newone.length() > (4 + indexsz + indexsz*indexsz) * 512) {
		puts("超过单个文件大小限制");
		return 0;
	}

	delDoc(inode);
	if (!saveDoc(inode, newone)) {
		saveDoc(inode, olddoc);
		puts("系统空间不足");
	}
	inode.docSize = newone.length();
	wrtInode(inodeId, inode);
	return 1;
}

bool isubDoc(int inodeId, int cut) {
	Inode inode = rdInode(inodeId);
	string olddoc = readDoc(inode);
	olddoc.substr(0, olddoc.size() - cut);
	delDoc(inode);
	saveDoc(inode, olddoc);
	inode.docSize = olddoc.length();
	wrtInode(inodeId, inode);
	return 1;
}

bool iaddMenu(int inodeId, Item item) {
	Inode inode = rdInode(inodeId);
	Menu menu = readMenu(inode);
	menu.add(item.name, item.addr);

	delDoc(inode);
	saveDoc(inode,menuToString(menu));
	inode.docSize = sizeof(menu);
	wrtInode(inodeId, inode);
	return 1;
}

bool idelMenu(int inodeId, Item item) {
	Inode inode = rdInode(inodeId);
	Menu menu = readMenu(inode);
	menu.remove(item.name);

	delDoc(inode);
	saveDoc(inode, menuToString(menu));
	inode.docSize = sizeof(menu);
	wrtInode(inodeId, inode);
	return 1;
}
#endif