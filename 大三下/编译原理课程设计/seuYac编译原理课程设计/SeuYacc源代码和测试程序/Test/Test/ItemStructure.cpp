#include "ItemStructure.h"



ItemStructure::ItemStructure()
{
}


ItemStructure::~ItemStructure()
{
}

Symbol::Symbol()
{
}

Symbol::Symbol(const Symbol & s):_name(s._name),_type(s._type),_val(s._val)
{
}

Symbol::Symbol(string name, int type)
{
	_name = name;
	_type = type;
}
Symbol::Symbol(string name, int type, string val)
{
	_name = name;
	_type = type;
	_val = val;
}
rule::rule()
{
}

rule::rule(const rule & r):Left(r.Left),Right(r.Right.begin(),r.Right.end())
{
}

rule::rule(Symbol & left, vector<Symbol>& right) : Left(left), Right(right)
{
}

int rule::addRight(Symbol s)
{
	Right.push_back(s);
	return 0;
}

int rule::clearRight()
{
	Right.clear();
	return 0;
}

Symbol & Symbol::operator=(Symbol & s2)
{
	_name = s2._name;
	_type = s2._type;
	_val = s2._val;
	return s2;
	// TODO: 在此处插入 return 语句
}

bool Symbol::isTermial()
{
	return _type;
}

ostream & operator<<(ostream & out, Symbol s)
{
	if (s._name=="")
		out << "epsilon";
	else
		out << s._name;
	return out;
}

bool operator<(const Symbol & s1, const Symbol & s2)
{
	return s1._name<s2._name;
}

bool operator==(const Symbol & s1, const Symbol & s2)
{
	return s1._name==s2._name&&s1._type==s2._type;;
}

ostream & operator<<(ostream & out, rule r)
{
	out << r.Left << "-->";
	for (auto it = r.Right.begin(); it != r.Right.end(); it++)
	{
		out << *it << " ";
	}
	out << "\n";
	return out;
}

bool operator==(const item & s1, const item & s2)
{
	return s1.ruleId==s2.ruleId&&s1.dot==s2.dot;
}

bool operator!=(const item & s1, const item & s2)
{

	return !(s1==s2);
}

bool operator<(const item & s1, const item & s2)
{
	if (s1.ruleId < s2.ruleId)
		return true;
	if (s1.ruleId == s2.ruleId&&s1.dot < s2.dot)
		return true;
	return false;
}

ostream & operator<<(ostream & out, item it)
{
	out << rs.rules[it.ruleId].Left << "->";
	int i = 0;
	for (i = 0; i < rs.rules[it.ruleId].Right.size(); i++)
	{
		if (i == it.dot)
			out << ".";
		out << rs.rules[it.ruleId].Right[i] << " ";
	}
	if (i == rs.rules[it.ruleId].Right.size()&&i== it.dot)
	{
		out << ".";
	}
	out << "\t\t{";
	for (Symbol s : it.predict)
	{
		out << s << ",";
	}
	out << "}";
	return out;
	// TODO: 在此处插入 return 语句
}

bool operator==(const itemSet & s1, const itemSet & s2)
{
	if (s1.items.size()!=s2.items.size())
		return false;
	for (auto it1 = s1.items.begin(), it2 = s2.items.begin(); it1 != s1.items.end(); it1++, it2++)
	{
		if (*it1!=*it2)
			return false;
	}
	return true;
}

ostream & operator<<(ostream & out, itemSet r)
{
	for (item it : r.items)
	{
		out << it << endl;
	}
	return out;
	// TODO: 在此处插入 return 语句
}

//ostream & operator<<(ostream & out, PDA r)
//{
//	out << "itemSet \n";
//	for (int i = 0; i < r.vItemSet.size(); i++)
//	{
//		out << i << "\n" << r.vItemSet[i];
//	}
//	out << endl;
//	out << "edgeSet \n";
//	for (auto it = r.edgeSet.begin(); it != r.edgeSet.end(); it++)
//	{
//		out << "<";
//		out << it->first.first << "," << it->first.second;
//		out << ">   ->   ";
//		out << it->second;
//		out << endl;
//	}
//	return out;
//	// TODO: 在此处插入 return 语句
//}

ruleSet::ruleSet()
{
	ruleNum = 0;
}

int ruleSet::addRule(rule & r)
{
	rules.push_back(r);
	queryMap[r.Left].push_back(ruleNum);
	ruleNum++;
	return ruleNum;
}

vector<int> ruleSet::QueryRuleByLeft(Symbol s)
{
	return queryMap[s];
}

vector<rule> ruleSet::MultQuery(vector<int> v)
{
	vector<rule> vv;
	for (auto it = v.begin(); it != v.end(); it++)
	{
		vv.push_back(rules[*it]);
	}
	return vector<rule>();
}

int ruleSet::calFF()
{
	unordered_map<string,int> hash;
	for (rule r : rules)
	{
		Symbols.insert(r.Left);
		for (Symbol t : r.Right)
			Symbols.insert(t);
	}
	Symbols.insert(Symbol("$", 1));
	//First
	for (Symbol s : Symbols)
	{
		if (s.isTermial())
		{
			Firstset[s].insert(s);
			continue;
		}
		vector<int> tempV = QueryRuleByLeft(s);
		set<Symbol> isused;
		for (int i = 0; i<tempV.size(); i++)
		{
			getFirstSet(tempV[i], Firstset[s], isused);
			if (rules[tempV[i]].Right.size() == 0)
			{
				Firstset[s].insert(Symbol("", 1));
			}
		}
	}

	return 0;
}

void ruleSet::clear()
{
	Symbols.clear();
	rules.clear();
	Firstset.clear();
	ruleNum = 0;
	queryMap.clear();
}

void ruleSet::printFirst()
{
	cout << "First:\n";
	for (Symbol s : Symbols)
	{
		cout << s << ": {";
		for (Symbol t : Firstset[s])
			cout << t << ",";
		cout << "}\n";
	}
}

bool ruleSet::getFirstSet(int ruleId, set<Symbol> &firstSet, set<Symbol> &isUsed)
{
	if (rules[ruleId].Right.size() == 0)
		return 0;
	else {
		int i = 0;
		if (rules[ruleId].Right[i].isTermial())
			firstSet.insert(rules[ruleId].Right[i]);
		else if (!rules[ruleId].Right[i].isTermial())
		{
			if (!isUsed.count(rules[ruleId].Right[i]))
			{
				isUsed.insert(rules[ruleId].Right[i]);
				
				vector <int> tv = QueryRuleByLeft(rules[ruleId].Right[i]);
				int c = 1;   
				for (auto p1 = tv.begin(); p1 != tv.end(); p1++)
				{
					if (!getFirstSet(*p1, firstSet, isUsed))
					{
						c = 0;
					}
				}
				while (c == 0)
				{
					i++;
					if (i<rules[ruleId].Right.size() &&
						(!rules[ruleId].Right[i].isTermial()) &&
						isUsed.count(rules[ruleId].Right[i]) == 0)
					{
						
						vector < int > tv2 = QueryRuleByLeft(rules[ruleId].Right[i]);
						c = 1;
						for (auto p2 = tv2.begin(); p2 != tv2.end(); p2++)
							if (!getFirstSet(*p2, firstSet, isUsed))
								c = 0;
					}
					else if (i<rules[ruleId].Right.size()&&rules[ruleId].Right[i].isTermial())
						firstSet.insert(rules[ruleId].Right[i]);
					if (i >= rules[ruleId].Right.size())
						c = 1;
				}
				if (i == rules[ruleId].Right.size())
				{
					firstSet.insert(Symbol("", 1));
				}
			}
		}
		return 1;
	}
}

item::item()
{
	dot = 0;
}

item::item(int id)
{
	ruleId = id;
	dot = 0;

}

item::item(int id, int dotpos)
{
	ruleId = id;
	dot = dotpos;
}

item::item(int id, int dotpos, set<Symbol> pre)
{
	ruleId = id;
	dot = dotpos;
	predict = pre;
}

void item::merge(item it)
{
	if (*this != it)
		return;
	for (Symbol sym : it.predict)
		predict.insert(sym);
}

bool item::isEOR()
{
	return rs.rules[ruleId].Right.size() <= dot;
}

bool item::deepeq(const item& s2)
{
	if (dot!=s2.dot||ruleId!=s2.ruleId||predict.size()!=s2.predict.size())
		return false;
	for (auto it1 = predict.begin(), it2 = s2.predict.begin(); it1 != predict.end(); it1++, it2++)
	{
		if (!(*it1 == *it2))
			return false;
	}
	return true;
}

Symbol item::nextSym()
{
	if (rs.rules[ruleId].Right.size() > dot + 1)
		return rs.rules[ruleId].Right[dot++];
	else
		return Symbol("null", -1);
}

Symbol item::getSym()
{
	if (rs.rules[ruleId].Right.size() > dot)
		return rs.rules[ruleId].Right[dot];
	else
		return Symbol("null", -1);
}

void item::getNextPre()
{
	if (rs.rules[ruleId].Right.size() <= dot + 1)
		return;
		//return set<Symbol>();
	predict = rs.Firstset[rs.rules[ruleId].Right[dot + 1]];
	//return rs.Firstset[rs.rules[ruleId].Right[dot + 1]];
}

itemSet::itemSet()
{
}

int itemSet::addItem(item it)
{
	items.insert(it);
	return 0;
}

int itemSet::closure()
{
	queue<item> q;
	for (item it : items)
	{
		q.push(it);
	}
	while (!q.empty())
	{
		//cout << *this;
		item t = q.front();
		q.pop();
		if (t.isEOR())
			continue;
		Symbol st = t.getSym();

		if (st.isTermial())
			continue;
		vector<Symbol> tRight = rs.rules[t.ruleId].Right;
		set<Symbol> tPre;
		int i;
		//if (rs.rules[t.ruleId].Left)
		for (i = t.dot+1; i < rs.rules[t.ruleId].Right.size(); i++)
		{
			if (rs.Firstset[rs.rules[t.ruleId].Right[i]].count(Symbol("", 1)) == 0)
			{
				for (Symbol s : rs.Firstset[rs.rules[t.ruleId].Right[i]])
				{
					tPre.insert(s);
				}
				break;
			}
			else
			{
				for (Symbol s : rs.Firstset[rs.rules[t.ruleId].Right[i]])
				{
					if (s == Symbol("", 1))
						continue;
					tPre.insert(s);
				}
				if (i == rs.rules[t.ruleId].Right.size() - 1)
				{
					for (Symbol s : t.predict)
					{
						tPre.insert(s);
					}
					break;
				}
			}
				
		}
		if (i == rs.rules[t.ruleId].Right.size())
		{
			for (Symbol s : t.predict)
			{
				tPre.insert(s);
			}
		}
		vector<int> vid = rs.QueryRuleByLeft(st);
		for (int j : vid)
		{
			item tempitem(j,0,tPre);
			if (items.find(tempitem) == items.end())
			{
				q.push(tempitem);
				items.insert(tempitem);
			}
			else
			{
				item tt = *items.find(tempitem);
				tempitem.merge(*items.find(tempitem));
				if (tt.deepeq(tempitem)==false)
				{
					items.erase(tempitem);
					items.insert(tempitem);
					q.push(tempitem);
				}
				
			}
		}
	}
	
	return 0;
}

int itemSet::merge(itemSet itemset)
{
	set<item> t;
	if (getString() == itemset.getString())
	{
		for (auto it1 = items.begin(), it2 = itemset.items.begin(); it1 != items.end(); it1++, it2++)
		{
			item temp = *it1;
			temp.merge(*it2);
			t.insert(temp);
		}
		items = t;
		return 0;
	}
	return -1;
}

void itemSet::clear()
{
	items.clear();
}

itemSet itemSet::shift(Symbol s)
{
	itemSet ret;
	for (item it : items)
	{
		if (it.getSym() == s)
		{

			item itt = it;
			itt.dot++;
			ret.addItem(itt);
		}
	}
	ret.closure();
	return ret;
}

string itemSet::getString()
{
	stringstream ss;
	for (item it : items)
	{
		ss << it.ruleId;
		ss << ":";
		ss << it.dot;
		ss << ";";
		ss << "pre";
		for (Symbol s : it.predict)
		{
			ss << s._name << ",";
		}
	}
	return ss.str();
}
//
//void PDA::clear()
//{
//	index = 1;
//	vItemSet.clear();
//	edgeSet.clear();
//	itemHash.clear();
//}
//
//void PDA::setStartRule(int ruleId)
//{
//	set<Symbol> v;
//	v.insert(Symbol("$", 1));
//	start.addItem(item(ruleId,0,v));
//}
//
//void PDA::genPDA()
//{
//	start.closure();
//	vItemSet.push_back(start);
//	itemHash[start.getString()] = index++;
//	queue<itemSet> q;
//	q.push(start);
//	while (!q.empty())
//	{
//		itemSet temp = q.front();
//		//cout << temp;
//		//cout << "______\n";
//		q.pop();
//		set<Symbol> cand;
//		for (item it : temp.items)
//		{
//			if (!it.isEOR())
//				cand.insert(it.getSym());
//		}
//		
//		for (Symbol s : cand)
//		{
//			itemSet p=temp.shift(s);
//			if (itemHash[p.getString()] <= 0)
//			{
//				vItemSet.push_back(p);
//				itemHash[p.getString()] = index++;
//				edgeSet[pair<int, Symbol>(itemHash[temp.getString()], s)] = itemHash[p.getString()];
//				q.push(p);
//			}
//			else
//			{
//				int ind=itemHash[p.getString()];
//				vItemSet[ind].merge(p);
//				edgeSet[pair<int, Symbol>(itemHash[temp.getString()], s)] = itemHash[p.getString()];
//			}
//		}
//	}
//}
//
//void PDA::ReduceAndConflict()//要多维护一个路径栈
//{
//	int* flag = new int[vItemSet.size()];
//	memset(flag, 0, sizeof(int)*vItemSet.size());
//	stack<int> mem;
//	mem.push(0);
//	recur(0, mem, flag);
//
//	delete []flag;
//}
//
//int PDA::recur(int state, stack<int>& mem,int*flag)
//{
//	stack<int> temp;
//	if (flag[state] == 1)
//		return 0;
//	flag[state] = 1;
//	itemSet is = vItemSet[mem.top()];
//	set<Symbol> cand;
//	vector<item> reduceV;
//	for (item it : is.items)
//	{
//		if (!it.isEOR())
//			cand.insert(it.getSym());
//		else
//			reduceV.push_back(it);
//	}
//	set<Symbol> produced;
//	if (reduceV.size() > 0) 
//	{
//		for (item it : reduceV)
//		{
//			int back = rs.rules[it.ruleId].Right.size();
//			for (int i = 0; i < back; i++)
//			{
//				temp.push(mem.top());
//				mem.pop();
//			}
//			if (it.ruleId == 0)
//			{
//				actionTable[pair<int, Symbol>(state, Symbol("$", 1))] = "acc";
//				produced.insert(Symbol("$", 1));
//			}
//			//int tar= edgeSet[pair<int, Symbol>(mem.top(), rs.rules[it.ruleId].Left)];
//			int tar= edgeSet[pair<int, Symbol>(mem.top(), rs.rules[it.ruleId].Left)];
//			stringstream ss;
//			ss << "r" << tar;
//			string ret = ss.str();
//			for (Symbol s : it.predict)
//			{
//				if (produced.find(s) == produced.end())
//				{
//					produced.insert(s);
//					actionTable[pair<int, Symbol>(state, s)] = ret;
//				}
//			}
//			for (int i = 0; i < back; i++)
//			{
//				mem.push(temp.top());
//				temp.pop();
//			}
//		}
//	}
//	for (Symbol s : cand)
//	{
//		int next = edgeSet[pair<int, Symbol>(state, s)];
//		stringstream ss;
//		ss << "s" << next;
//		string ret = ss.str();
//		if (produced.find(s) == produced.end())
//		{
//			if (s.isTermial())
//			{
//					actionTable[pair<int, Symbol>(state, s)] = ret;
//			}	
//			else
//				gotoTable[pair<int, Symbol>(state, s)] = next;
//		}
//		else
//		{
//			int prepri = -1;
//			string tempstr;
//			for (item it : reduceV)
//			{
//				for (auto itt = rs.rules[it.ruleId].Right.rbegin(); itt != rs.rules[it.ruleId].Right.rend(); itt++)
//				{
//					if (rs.priority[itt->_name] <= 0)
//					{
//						continue;
//					}
//					else
//					{
//						prepri = rs.priority[itt->_name];
//						tempstr = itt->_name;
//						break;
//					}
//				}
//			}
//
//
//			if (rs.Right.find(s._name) != rs.Right.end())//右结合倾向移进
//			{
//				if (rs.priority[s._name]>= prepri)
//					actionTable[pair<int, Symbol>(state, s)] = ret;
//			}
//			if (rs.Left.find(s._name) != rs.Left.end())//左结合倾向规约
//			{
//				if (rs.priority[s._name] > prepri)
//					actionTable[pair<int, Symbol>(state, s)] = ret;
//			}
//		}
//		
//		mem.push(next);
//		recur(next, mem, flag);
//		mem.pop();
//	}
//	return 0;
//}
