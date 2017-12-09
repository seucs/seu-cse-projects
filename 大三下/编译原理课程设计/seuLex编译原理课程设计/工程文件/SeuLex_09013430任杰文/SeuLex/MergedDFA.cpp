#include "MergedNFA.h"

MergedNFA::MergedNFA ( const vector<NFA*> &nfas ) {
	//识别模式Token i 的最终状态的集合
	finalStates.resize( nfas.size() );
	//当前已合并状态数（添加一个初始状态）
	numStateOfNFA = 1;

	for ( int i=0; i<nfas.size(); i++ ) {
		//NFA重新编号
		nfas[i]->shift( numStateOfNFA );
		//计算合并NFA状态数
		numStateOfNFA += nfas[i]->numStateOfNFA;
		//设置识别TOKEN的接收态集合
		finalStates[i] = numStateOfNFA-1;
	}
	//构造新的状态转化表
	transTabOfNFA = new Table(numStateOfNFA);
	int curNumOfState = 1;
	for ( int i=0; i<nfas.size(); i++ ) {
		//添加生成NFA初态到各NFA初态的EPSILON边
		transTabOfNFA->add(0, EPSILON, curNumOfState);
		//复制其它转化函数
		for ( int j = 0; j < nfas[i]->numStateOfNFA;  j++ ) {
			transTabOfNFA->copyLineByRef ( nfas[i]->transTabOfNFA, j, curNumOfState);
			curNumOfState++;
		}
	}
}
//返回当前NFA状态识别的Token编号，低编号具有高优先级
int MergedNFA::statePatten(int s) 
{
	for ( int i = 0; i < finalStates.size(); i++ ) {
		if ( finalStates[i] == s)
			return i;
	}
	return -1;
}

