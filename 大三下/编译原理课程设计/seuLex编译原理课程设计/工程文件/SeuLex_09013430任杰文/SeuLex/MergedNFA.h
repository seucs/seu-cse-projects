#ifndef MERGEDNFA_H
#define MERGEDNFA_H
#include "headers.h"
#include "NFA.h"

//派生自NFA
class MergedNFA :public NFA 
{
public:
	//合并NFA
	MergedNFA ( const vector<NFA*> &nfas );
	//返回当前NFA状态识别的Token编号，低编号具有高优先级
	int statePatten(int s);
private:
	//识别模式Token i 的最终状态的集合
	vector<int> finalStates;
};
#endif