#include "NFA.h"

//构造函数
NFA::NFA() {
}
NFA::NFA( int _numState ) {
	numStateOfNFA = _numState;
	transTabOfNFA = new Table(numStateOfNFA);
}
NFA::NFA( string regExpr , const map<string,string> &regDefPart) {
	//中缀转后缀,操作符用header中的内码替换
	FormatRegExp *ipt = new FormatRegExp(regDefPart);
	string postReg = ipt->postfix( regExpr );

	//遍历后缀表达式，构造NFA
	stack<NFA* > NFAStack;
	NFA* curNFA_1;
	NFA *curNFA_2;
	char curSymbol;
	for ( int i=0; i < postReg.length(); i++ ) 
	{
		curSymbol = postReg[i];
		//如果当前为运算符（ 连接、或、闭包，正闭包，或空），则出栈NFA，计算结果后再入栈
		//如果当前为简单字符，则构造一个只含始态、终态的NFA，并入栈
		if ( curSymbol == AND ) {
			curNFA_1 = NFAStack.top();
			NFAStack.pop();
			curNFA_2 = NFAStack.top();
			NFAStack.pop();
			NFAStack.push( and(curNFA_2, curNFA_1) );
		} else if ( curSymbol == OR ) {
			curNFA_1 = NFAStack.top();
			NFAStack.pop();
			curNFA_2 = NFAStack.top();
			NFAStack.pop();
			NFAStack.push(or(curNFA_2,curNFA_1));
		} else if ( curSymbol == STAR ) {
			curNFA_1 = NFAStack.top();
			NFAStack.pop();
			NFAStack.push( star(curNFA_1) );
		} else if ( curSymbol == ONE_OR_MORE ){
			curNFA_1 = NFAStack.top();
			NFAStack.pop();
			NFAStack.push( oneMore(curNFA_1) );
		} else if ( curSymbol == ONE_OR_NONE ){
			curNFA_1 = NFAStack.top();
			NFAStack.pop();
			NFAStack.push(oneNone(curNFA_1));
		}else {
			curNFA_1 = new NFA(2);
			curNFA_1->transTabOfNFA->add(0, curSymbol, 1);
			NFAStack.push(curNFA_1);
		}
	}
	//最终结果NFA
	curNFA_1 = NFAStack.top();
	NFAStack.pop();
	//NFA转化表及状态数
	transTabOfNFA = curNFA_1->transTabOfNFA;
	numStateOfNFA = transTabOfNFA->getNumRows();
}

//NFA运算
NFA* NFA::or(NFA *m, NFA *n) 
{
	//添加始态、终态
	NFA *res = new NFA( m->numStateOfNFA + n->numStateOfNFA + 2 );
	//重新给状态编号，并重排状态转移表
	m->shift(1);
	n->shift( m->numStateOfNFA+1 );
	for ( int i=0; i<m->numStateOfNFA; i++ ){
		res->transTabOfNFA->copyLineByRef(m->transTabOfNFA, i, i+1);
	}
	for ( int i=0; i<n->numStateOfNFA; i++) {
		res->transTabOfNFA->copyLineByRef(n->transTabOfNFA, i, m->numStateOfNFA+1+i);
	}
	//添加EPSILON边
	res->transTabOfNFA->add(0, EPSILON, 1);
	res->transTabOfNFA->add(0, EPSILON, m->numStateOfNFA+1);
	res->transTabOfNFA->add(m->numStateOfNFA, EPSILON, res->numStateOfNFA-1);
	res->transTabOfNFA->add(res->numStateOfNFA-2, EPSILON, res->numStateOfNFA-1);
	return res;
}

NFA* NFA::and(NFA *m, NFA *n) 
{
	//合并m终态与n始态
	NFA *res = new NFA(m->numStateOfNFA+n->numStateOfNFA-1);
	//重新给状态编号，并重排状态转移表
	n->shift(m->numStateOfNFA-1);
	for ( int i=0; i<m->numStateOfNFA; i++ ) {
		res->transTabOfNFA->copyLineByRef(m->transTabOfNFA, i, i);
	}
	for ( int i=0; i<n->numStateOfNFA; i++) {
		res->transTabOfNFA->copyLineByRef(n->transTabOfNFA, i, m->numStateOfNFA+i-1);
	}
	return res;
}

NFA* NFA::star(NFA *m) 
{
	//添加始态、终态
	NFA *res = new NFA(m->numStateOfNFA+2);
	//重新给状态编号，并重排状态转移表
	m->shift(1);
	for ( int i=0; i<m->numStateOfNFA; i++ ){
		res->transTabOfNFA->copyLineByRef(m->transTabOfNFA, i, i+1);
	}
	//添加EPSILON边
	res->transTabOfNFA->add(0, EPSILON, 1);
	res->transTabOfNFA->add(0, EPSILON, res->numStateOfNFA-1);
	res->transTabOfNFA->add(res->numStateOfNFA-2, EPSILON, 1);
	res->transTabOfNFA->add(res->numStateOfNFA-2, EPSILON, res->numStateOfNFA-1);
	return res;
}

NFA* NFA::oneMore(NFA *m) 
{
	//添加始态、终态
	NFA *res = new NFA(m->numStateOfNFA+2);
	//重新给状态编号，并重排状态转移表
	m->shift(1);
	for ( int i=0; i<m->numStateOfNFA; i++ ){
		res->transTabOfNFA->copyLineByRef(m->transTabOfNFA, i, i+1);
	}
	//添加EPSILON边
	res->transTabOfNFA->add(0, EPSILON, 1);
	res->transTabOfNFA->add(res->numStateOfNFA-2, EPSILON, 1);
	res->transTabOfNFA->add(res->numStateOfNFA-2, EPSILON, res->numStateOfNFA-1);
	return res;
}

NFA* NFA::oneNone(NFA *m) {
	//clock_t t1 = clock();

	m->transTabOfNFA->add(0, EPSILON, m->numStateOfNFA-1);
	//clock_t t2 = clock();
	//cout<<"onenone"<<t2-t1<<endl;
	return m;
}
//将NFA状态转化表中状态序号全部加 s
void NFA::shift(int s) {
	//遍历所有状态
	for ( int i = 0; i < numStateOfNFA; i++ ) {
		//遍历所有符号
		for ( int symb = 0; symb < NUM_OF_COLUMNS; symb++ ) {
			//获取状态 i 接收符号symb后转移到的状态集合
			hash_set<int>* items = transTabOfNFA->get( i,symb );
			if ( items == NULL) {
				continue;
			}
			//将原状态集合中状态序号全部加 s 
			hash_set<int> *newItems = new hash_set<int>();
			hash_set<int>::iterator iter = items->begin();
			while(iter != items->end()){
				newItems->insert( *iter + s );
				iter++;
			}
			delete items;
			//更新状态转化表第 i+1 行第 symb+1列
			transTabOfNFA->set(i, symb, newItems);
		}
	}
}
//返回NFA状态转换表
Table* NFA::getTrsTbl() 
{
	return transTabOfNFA;
}

//获取状态state 接收符号symbol后转移到的状态集合
hash_set<int>* NFA::move( int state, char symbol ) 
{
	return transTabOfNFA->get(state, symbol);
}