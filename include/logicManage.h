#ifndef LOGICMANAGE
#define LOGICMANAGE
#include"superblock.h"
#include"freeBlockManage.h"
#include"freeInodeManage.h"
#include"docTree.h"
#include"diskRW.h"

//��װϵͳ
void fresh() {
	sprblk.freeBlockStr = totBlockInit();
	sprblk.freeInodeStr = totInodeInit();
	sprblk.root = iallocMenu(-1,CurUser("root","root"));

}

//��������
//������ʼ��
bool getInit() {
	sprblk = rdSpblk();
	return 1;
}

//�ر�ǰ������Ҫ����
bool getDestroy() {
	wrtSpblk(sprblk);
	return 1;
}


#endif