#ifndef NFA_H
#define NFA_H
#include "headers.h"
#include "TransTable.h"
#include "FormatRegExp.h"



//不确定有限自动机
class NFA 
{
public:
	//构造函数
	NFA();
	NFA( int _numState );
	NFA( string regExpr , const map<string,string> &regDefPart);
	
	//NFA运算
	NFA* or(NFA *m, NFA *n);
	NFA* and(NFA *m, NFA *n);
	NFA* star(NFA *m);
	NFA* oneMore(NFA *m);
	NFA* oneNone(NFA *m);

	//将NFA状态转化表中状态序号全部加 s
	void shift(int s);
	//返回NFA状态转换表
	Table* getTrsTbl();

	//获取状态state 接收符号symbol后转移到的状态集合
	hash_set<int>* move( int state, char symbol );
public:
	//状态转化表
	Table *transTabOfNFA;
	//NFA状态数
	int numStateOfNFA;
};

#endif