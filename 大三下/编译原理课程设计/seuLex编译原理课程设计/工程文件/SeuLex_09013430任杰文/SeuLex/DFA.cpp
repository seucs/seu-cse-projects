#include "DFA.h"

//用NFA构造DFA
DFA::DFA( MergedNFA *pnfa) {
	//构造数据NFA
	pMergedNFA = pnfa;
	//初始状态默认为0
	enterState = 0;

	//定义：DFA闭包、DFA闭包标号、闭包之间的转化映射关系（表），vector为128维，对应128个ASCLL码
	list<hash_set<int>* > UnmarkedDstates;
	vector<hash_set<int> > Dstates;
	hash_map<hash_set<int>*, vector<hash_set<int>* >* > Dtran;

	//初始化：将EPSILON闭包标号，作为DFA的状态、添加DFA状态、添加闭包间映射关系
	Dstates.push_back( *epsilonClosure(0) );
	UnmarkedDstates.push_front( epsilonClosure(0) );
	Dtran[ epsilonClosure(0) ] = new vector<hash_set<int>* >(NUM_OF_COLUMNS,NULL);

	//当存在新的DFA状态时，遍历所有现有DFA状态，为每一个DFA状态添加转化函数，并把出现的新状态加入DFA集合中：
	hash_set<int>* curDFAState;
	while ( !UnmarkedDstates.empty() ) {
		curDFAState = UnmarkedDstates.front();
		UnmarkedDstates.erase( UnmarkedDstates.begin() );

		//把出现的新状态加入DFA集合中：
		hash_set<int> *U;
		for ( int symbol = 1; symbol < NUM_OF_COLUMNS; symbol++ ) 
		{
			//求当前转换的EPSILON闭包
			U = epsilonClosure( move( curDFAState, symbol ) );
			if (  U->empty() )
				continue;
			//加入新的EPSILON闭包作为DFA状态
			if ( find( Dstates.begin(), Dstates.end(), *U) == Dstates.end() ) {
				//标号、加入新的DFA状态、添加闭包间映射关系
				Dstates.push_back( *U );
				UnmarkedDstates.push_front(U);
				vector<hash_set<int>* >* v = new vector<hash_set<int>* >(NUM_OF_COLUMNS,NULL);
				Dtran[U] = v;
			}
			//添加闭包间映射关系
			(*(Dtran[curDFAState]))[symbol] = U;
		}		
	}

	//DFA状态数、初始化状态转化表、初始化识别TOKEN表
	numStatesOfDFA = Dstates.size();
	transTabOfDFA = new vector<vector<int> >( numStatesOfDFA, vector<int>(NUM_OF_COLUMNS,-1) );
	finalStatePatten = new vector<int>(numStatesOfDFA,-1);

	//遍历所有DFAState
	hash_map<hash_set<int>*, vector<hash_set<int>* >* >::iterator it = Dtran.begin();
	hash_set<int>* curState;
	while( it != Dtran.end() )
	{
		curState = (it++)->first;
		int tagOfCurState =  find(Dstates.begin(), Dstates.end(), *curState)-Dstates.begin();
		//标记当前DFA状态所识别的token
		(*finalStatePatten)[tagOfCurState] = statePatten( curState );
		(*finalStatePatten)[tagOfCurState] = statePatten( curState );

		//构造状态转化表
		vector<hash_set<int>* >* TransOfCurState = Dtran[curState];
		for ( int symbol = 0; symbol < NUM_OF_COLUMNS; symbol++ )
		{
			if ( (*TransOfCurState)[symbol] != NULL &&  !(*TransOfCurState)[symbol]->empty() )
				(*transTabOfDFA)[tagOfCurState][symbol] = find(Dstates.begin(), Dstates.end(), *(*TransOfCurState)[symbol])-Dstates.begin();
		}
	}
}
//返回当前DFA状态识别Token编号，低编号具有高优先级
int DFA::statePatten( hash_set<int>* curStateOfDFA )
{ 
	int minState = pMergedNFA->numStateOfNFA;
	int pid = -1;
	//遍历此DFA状态集合中的NFA状态
	hash_set<int>::iterator it = curStateOfDFA->begin();
	while ( it != curStateOfDFA->end() )
	{
		int currentState = *(it++);
		//当前状态识别token标号
		int curpid = pMergedNFA->statePatten(currentState);
		if ( curpid > -1 && currentState < minState ) {
			pid = curpid;
			minState = currentState;
		}
	}
	return pid;
}
//返回集合T,接收a后转化状态集
hash_set<int>* DFA::move(hash_set<int>* T, char a) 
{
	hash_set<int>* res = new hash_set<int>();
	hash_set<int>::iterator it = T->begin();
	while ( it != T->end() ) 
	{
		hash_set<int>* t = pMergedNFA->move(*it, a);
		it++;
		if( t == NULL)
			continue;
		res->insert(t->begin(), t->end());
	}
	return res;
}
//返回状态 i 的EPSILON闭包
hash_set<int>* DFA::epsilonClosure ( int i ) 
{
	//计算过 i 状态的EPSILON闭包，直接返回
	if ( espClsTbl.count(i) ) 
		return (espClsTbl)[i];

	//深度优先遍历，求EPSILON闭包
	hash_set<int>* epsCls = new hash_set<int>();
	epsCls->insert(i);
	stack<int> s;
	s.push(i);
	while ( !s.empty() ) 
	{
		int t = s.top();
		s.pop();

		//求当前状态的EPSILON转化装态
		hash_set<int> *epsTrs = pMergedNFA->move(t, EPSILON);
		if(epsTrs == NULL)
			continue;
		//将当前状态的EPSILON转化状态并入最终EPSILON闭包
		hash_set<int>::iterator itEspTrs = epsTrs->begin();
		while ( itEspTrs != epsTrs->end() ) 
		{
			int u = *itEspTrs;
			itEspTrs++;
			//判断当前状态是否已遍历过
			if ( !epsCls->count(u) )
				s.push(u);
			epsCls->insert(u);
		}
	}
	espClsTbl[i] = epsCls;

	return epsCls;
}
//返回一个NFA状态集合的EPSILON闭包
hash_set<int>* DFA::epsilonClosure( hash_set<int>* curSetOfNFAStates ) 
{
	hash_set<int>* res = curSetOfNFAStates;
	//遍历当前状态集，将每个状态的EPSILON闭包加入返回结果中
	hash_set<int>::iterator i = curSetOfNFAStates->begin();
	while ( i != curSetOfNFAStates->end() ) 
	{
		hash_set<int>* t = epsilonClosure( *i );
		res->insert(t->begin(), t->end());
		i++;
	}
	return res;
}
//返回DFA状态数
int DFA::getNumStates() {
	return numStatesOfDFA;
}
//返回状态转化表
vector<vector<int > >* DFA::getTable() {
	return transTabOfDFA;
}
//返回DFA识别模式表
vector<int >* DFA::getStatePattern() {
	return finalStatePatten;
}
//DFA最小化
void DFA::minimize() {
	//计算DFA可识别Token数
	int numOfToken = -1;
	for ( int i = 0; i<numStatesOfDFA; i++ ) {
		if ( (*finalStatePatten)[i] > numOfToken ) {
			numOfToken =  (*finalStatePatten)[i];
		}
	}
	numOfToken++;

	//定义：最小化DFA状态集合及其编号、原DFA状态映射到最小化DFA的某个状态、
	vector<set<int>* >* statesOfMinimizedDFA = new vector<set<int>* >();
	map< int , int > oriStateToMinimizedState;

	//初始化：初始化非终态与终态集合
	set<int>* nonfinalStates = new set<int>();
	vector<set<int>* >* finalStates = new vector<set<int>* >(numOfToken, NULL);
	for( int i = 0; i < finalStates->size(); i++)
		(*finalStates)[i] = new set<int>();
	for ( int curState = 0; curState < numStatesOfDFA; curState++ ){
		if ( (*finalStatePatten)[curState] < 0 ) 
		{
			//非终态
			nonfinalStates->insert( curState );
		} else{
			//对每一个终态根据识别Token不同划分到一个state集合
			(*finalStates)[ (*finalStatePatten)[curState] ]->insert(curState);
		}
	}

	//插入最小化DFA初始状态集，并初始化新旧状态映射表
	if ( !nonfinalStates->empty()) {
		statesOfMinimizedDFA->push_back(nonfinalStates);
		for(set<int>::iterator it = nonfinalStates->begin(); it != nonfinalStates->end();  it++)
			oriStateToMinimizedState[ *it ] = statesOfMinimizedDFA->size() - 1;
	}
	for ( int i = 0; i < numOfToken; i++ ) {
		if ( !(*finalStates)[i]->empty() ) {
			statesOfMinimizedDFA->push_back( (*finalStates)[i] );
			for(set<int>::iterator it = (*finalStates)[i]->begin();  it != (*finalStates)[i]->end(); it++)
				oriStateToMinimizedState[ *it ] = statesOfMinimizedDFA->size() - 1;
		}
	}
	//自顶向下划分
	while ( partition(statesOfMinimizedDFA,oriStateToMinimizedState) );

	//构造新状态转化表、新状态与识别Token映射表
	vector<vector<int> >* newTrsTbl = new vector<vector<int> >(statesOfMinimizedDFA->size(), vector<int>(NUM_OF_COLUMNS,-1));
	vector<int>* newStatePattern = new vector<int>(statesOfMinimizedDFA->size(),-1);
	for( vector<set<int>* >::iterator itState = statesOfMinimizedDFA->begin();   itState != statesOfMinimizedDFA->end() ;itState++)
	{
		set<int>::iterator it = (*itState)->begin();
		int oldNum  = *it;
		int newNum = oriStateToMinimizedState[oldNum];
		(*newStatePattern)[newNum] = (*finalStatePatten)[oldNum];
		//构造新的状态转化表
		for ( int symb = 0; symb < NUM_OF_COLUMNS; symb++ ) {
			if(  (*transTabOfDFA)[ oldNum ][ symb ]  == -1)
				(*newTrsTbl)[newNum][symb] = -1;
			else
				(*newTrsTbl)[newNum][symb] = oriStateToMinimizedState [ (*transTabOfDFA)[ oldNum ][ symb ] ];
		}
	}
	//更新DFA，有内存泄漏，不管了
	enterState = oriStateToMinimizedState[0];
	numStatesOfDFA = statesOfMinimizedDFA->size();
	transTabOfDFA = newTrsTbl;
	finalStatePatten = newStatePattern;
}
//根据出边划分子集
bool DFA::partition( vector<set<int>* >* statesOfMinimizedDFA, map<int,int>& oriStateToMinimizedState) 
{
	//标记当前迭代是否有新的划分
	bool partitioned = false;

	//遍历所有状态集
	for( int curS = 0; curS < statesOfMinimizedDFA->size(); curS++)
	{
		set<int>* curState = (*statesOfMinimizedDFA)[curS];
		//当前状态集不能再划分
		if ( curState->size() <= 1 )
			continue;
		//遍历当前状态集中所有状态的所有可能出边
		for ( int c = 1; c < NUM_OF_COLUMNS; c++ ) 
		{
			if( c == 127)
				int t = 0;
			//映射到某个最小化DFA状态中 的 DFA状态集合
			map< int ,set<int> > partByc;
			for( set<int>::iterator it = curState->begin(); it != curState->end() ; it++)
			{
				//是否接受c
				if ( (*transTabOfDFA)[*it][c] == -1 ){
					partByc[-1].insert(*it);
				}else{
					partByc[  oriStateToMinimizedState[ (*transTabOfDFA)[*it][c] ] ] .insert(*it);
				}
			}
			//更具映射关系划分新的DFA集合
			if( partByc.size() > 1)
			{
				partitioned = true;
				//从第二个集合开始创建新的DFA集合
				map< int ,set<int> >::iterator it  = partByc.begin();
				for( it++; it != partByc.end(); it++)
				{
					set<int> *newState = new set<int>();
					for( set<int>::iterator i = (*it).second.begin(); i != (*it).second.end(); i++)
					{
						newState->insert( *i );
						curState->erase( *i );
					}
					//插入新的DFA集合
					statesOfMinimizedDFA->push_back(newState);
					for( set<int>::iterator j = newState->begin();  j != newState->end(); j++)
						oriStateToMinimizedState[ *j ] = statesOfMinimizedDFA->size() - 1;
				}
			}
		}
	}
	return partitioned;
}
//获取DFA初始状态
int DFA::getEnterState()
{
	return enterState;
}