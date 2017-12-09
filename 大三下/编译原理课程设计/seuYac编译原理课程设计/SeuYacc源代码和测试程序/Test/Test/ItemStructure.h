#pragma once
#include <map>
#include <vector>
#include <iostream>
#include <string>
#include <set>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <fstream>
#include <stack>
using namespace std;
class ItemStructure
{
public:
	ItemStructure();
	~ItemStructure();
};
class Symbol//终结符与非终结符
{
public:
	string _name;
	int _type;//0非终结符，1终结符
	string _val;//语义值
	Symbol();
	Symbol(const Symbol& s);
	Symbol(string name, int type);
	Symbol(string name, int type, string val);
	Symbol& operator = (Symbol& s2);
	bool isTermial();
	friend ostream& operator << (ostream& out, Symbol s);
	friend bool operator < (const Symbol& s1, const Symbol& s2);
	friend bool operator == (const Symbol& s1, const Symbol& s2);
};

class rule//单条规则
{
public:
	Symbol Left;
	vector<Symbol> Right;
	rule();
	rule(const rule& r);
	rule(Symbol& left, vector<Symbol>& right);
	int addRight(Symbol s);
	int clearRight();
	friend ostream& operator << (ostream& out, rule r);
};

class ruleSet//规则集 id->rule
{
public:
	vector<rule> rules;
	map<Symbol, set<Symbol>> Firstset;
	ruleSet();
	int addRule(rule& r);
	vector<int> QueryRuleByLeft(Symbol s);
	vector<rule> MultQuery(vector<int> v);
	int calFF();
	void clear();
	void printFirst();
	bool getFirstSet(int ruleId, set<Symbol> &firstSet, set<Symbol> &isUsed);
	set<string> Token;
	//结合
	set<string> Left;
	set<string> Right;
	set<string> Nonassoc;
	//优先级
	map<string, int> priority;
	set<Symbol> Symbols;
private:
	map<Symbol, vector<int>> queryMap;
	
	int ruleNum;
};

class item
{
public:
	item();
	item(int id);
	item(int id,int dotpos);
	item(int id, int dotpos,set<Symbol> pre);
	bool deepeq(const item& s2);
	void merge(item it);
	friend bool operator == (const item& s1, const item& s2);
	friend bool operator != (const item& s1, const item& s2);
	friend bool operator < (const item& s1, const item& s2);
	friend ostream& operator << (ostream& out, item it);
	bool isEOR();
	Symbol nextSym();
	Symbol getSym();
	void getNextPre();
	int ruleId;
	int dot;
	set<Symbol> predict;
private:
	

};

class itemSet
{
public:
	itemSet();
	int addItem(item);
	int closure();
	int merge(itemSet itemset);
	void clear();
	itemSet shift(Symbol s);
	friend bool operator == (const itemSet& s1, const itemSet& s2);
	friend ostream& operator << (ostream& out, itemSet r);
	string getString();//用于hash
	set<item> items;
private:
	
};

//class PDA
//{
//public:
//	vector<itemSet> vItemSet;//id->ItemSet
//	map<pair<int, Symbol>, int> edgeSet;//<id,Symbol>->id
//	map<pair<int, Symbol>, int> reduceSet;
//	map<pair<int, Symbol>, string> actionTable;
//	map<pair<int, Symbol>, int> gotoTable;
//	unordered_map<string, int> itemHash;
//	itemSet start;
//	void clear();
//	void setStartRule(int ruleId);
//	void genPDA();
//	void ReduceAndConflict();
//	friend ostream& operator << (ostream& out, PDA r);
//	int index;
//private:
//	int recur(int state, stack<int>& mem,int* flag);
//};


extern ruleSet rs;
//extern PDA pda;