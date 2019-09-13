/*
	���ﶨ��һЩ�����Ӱ˵����ݽṹ�ͺ���
*/

#ifndef EXTRAFUN
#define EXTRAFUN
#include<cstring>
#include<string>
using namespace std;



//�Զ���ջ ���ڳ����� Ϊ�˹̶��Ĵ�С
struct Stack {
	int sz,tp; //ջ���ô�С��ջ��ָ��
	int sta[32]; //ջ����
	Stack(int sz = 0,int tp=0) 
		:sz(sz),tp(tp){
		memset(sta, 0, sizeof(sta));
	}
	//�ж�ջ�Ĵ�С
	int size() { return tp; }
	//�ж�ջ�Ƿ�Ϊ��
	bool empty() {
		return tp == 0;
	}
	//ѹ��һ��Ԫ��
	bool push(int x) {
		if (tp == sz) return 0;
		sta[tp++] = x;
		return 1;
	}
	//����һ��Ԫ��
	bool pop() {
		if (tp == 0) return 0;
		tp--;
		return 1;
	}
	//����ջ��Ԫ��
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