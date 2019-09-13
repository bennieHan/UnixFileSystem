/*
	这里定义一些杂七杂八的数据结构和函数
*/

#ifndef EXTRAFUN
#define EXTRAFUN
#include<cstring>
#include<string>
using namespace std;



//自定义栈 用于超级块 为了固定的大小
struct Stack {
	int sz,tp; //栈可用大小，栈顶指针
	int sta[32]; //栈数组
	Stack(int sz = 0,int tp=0) 
		:sz(sz),tp(tp){
		memset(sta, 0, sizeof(sta));
	}
	//判断栈的大小
	int size() { return tp; }
	//判断栈是否为空
	bool empty() {
		return tp == 0;
	}
	//压入一个元素
	bool push(int x) {
		if (tp == sz) return 0;
		sta[tp++] = x;
		return 1;
	}
	//弹出一个元素
	bool pop() {
		if (tp == 0) return 0;
		tp--;
		return 1;
	}
	//返回栈顶元素
	int top() {
		if (tp==0) { return 0; }
		return sta[tp - 1];
	}
};

void _swap(int &a, int &b) {
	int tmp = a;
	a = b;
	b = tmp;
}
#endif