#ifndef LOGICMANAGE
#define LOGICMANAGE
#include"superblock.h"
#include"freeBlockManage.h"
#include"freeInodeManage.h"
#include"docTree.h"
#include"diskRW.h"

//重装系统
void fresh() {
	sprblk.freeBlockStr = totBlockInit();
	sprblk.freeInodeStr = totInodeInit();
	sprblk.root = iallocMenu(-1,CurUser("root","root"));

}

//重新启动
//开机初始化
bool getInit() {
	sprblk = rdSpblk();
	return 1;
}

//关闭前保存重要数据
bool getDestroy() {
	wrtSpblk(sprblk);
	return 1;
}


#endif