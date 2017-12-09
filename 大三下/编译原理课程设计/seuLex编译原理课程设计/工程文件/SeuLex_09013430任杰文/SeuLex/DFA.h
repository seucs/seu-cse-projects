#ifndef DFA_H
#define DFA_H
#include "headers.h"
#include "MergedNFA.h"

class DFA {
public:
	//用NFA构造DFA
	DFA( MergedNFA *pnfa);
	//返回当前DFA状态识别Token编号，低编号具有高优先级
	int statePatten( hash_set<int>* curStateOfDFA );
	//返回集合T,接收a后转化状态集
	hash_set<int>* move(hash_set<int>* T, char a);
	//返回状态 i 的EPSILON闭包
	hash_set<int>* epsilonClosure ( int i );
	//返回一个NFA状态集合的EPSILON闭包
	hash_set<int>* epsilonClosure( hash_set<int>* curSetOfNFAStates );
	//返回DFA状态数
	int getNumStates();
	//返回状态转化表
	vector<vector<int > >* getTable();
	//返回DFA识别模式表
	vector<int >* getStatePattern();

	//DFA最小化
	void minimize();
	//根据出边划分子集
	bool partition( vector<set<int>* >* statesOfMinimizedDFA, map<int,int>& oriStateToMinimizedState);
	//获取DFA初始状态
	int getEnterState();
private:
	//待转化pMergedNFA
	MergedNFA *pMergedNFA;
	//pMergedNFA各状态与其闭包的映射表
	hash_map<int, hash_set<int>* > espClsTbl;

	
	//DFA状态数
	int numStatesOfDFA;
	//DFA初始状态
	int enterState;
	//DFA状态转化表
	vector<vector<int> >* transTabOfDFA;
	//当前状态所接收token标号
	vector<int>* finalStatePatten;
};
#endif
