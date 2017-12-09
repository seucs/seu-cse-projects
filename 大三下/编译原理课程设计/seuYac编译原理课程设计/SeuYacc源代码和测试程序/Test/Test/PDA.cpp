#include "PDA.h"
#include "translation.h"
PDA::PDA()
{
	Symbol s("$",1);
	PDstack.push(s);
	reader = 0;
}
PDA::PDA(ruleSet RS)
{
	reader = 0;
	Symbol s("$",1);
	PDstack.push(s);
	sentence.clear();
	rs = RS;
}
PDA::PDA(vector<rule> RS)
{
	reader = 0;
	Symbol s("$",1);
	PDstack.push(s);
	sentence.clear();
	for (int i = 0;i<RS.size();i++)
		rs.addRule(RS[i]);
}
void PDA::input(vector<Symbol> s)
{
	sentence = s;
}
void PDA::shift()
{
	
	PDstack.push(sentence[reader]);
	SDTStack.push(sentence[reader]._val);
	reader++;
	if (isError())
		return;
	/*if (isSuccess())
		success();*/
	
}
Symbol PDA::reduce(rule r)
{
	Symbol s;
	for (int i = r.Right.size()-1;i>=0;i--)
	{
		s = PDstack.top();
		if (!(s == r.Right[i]))
		{
			error(2);
		}
		else
			PDstack.pop();
	}
	PDstack.push(r.Left);
	if (isError())
		error(1);
	/*if (isSuccess())
		success();*/
	return r.Left;
}
bool PDA::isError()
{
	if (reader>sentence.size())
	{
		return true;
	}
	return false;
}
bool PDA::isSuccess()
{
	return (PDstack.top()._name == rs.rules[0].Right[0]._name && sentence[reader]._name == "$");
}
void PDA::success()
{
	ofstream fout("ParsingLog.txt");
	quadOutput();
	cout<<"This sentence has been accepted"<<endl;
	fout<<"This sentence has been accepted"<<endl;
	system("pause");
}
void PDA::error(int errorID)
{
	ofstream fout("ParsingLog.txt");
	cout<<"ERROR!"<<endl;
	cout<<"More information in ParsingLog.txt"<<endl;
	fout<<"ERROR!"<<endl;
	fout<<"Reader at : "<<sentence[reader]._val<<endl;
	fout<<"Error info:";
	switch (errorID)
	{
	case 0:
		fout<<"Shift into error state!"<<endl;
		break;
	case 1:
		fout<<"Reader overflow!"<<endl;
		break;
	case 2:
		fout<<"Wrong matching with the rule!"<<endl;
		break;
	case 3:
		fout<<"Wrong state expression!"<<endl;
		break;
	default:
		break;
	}
	fout<<"state stack:"<<endl;
	for (int i = 0;i<stateStack.size();i++)
	{
		fout<<stateStack.top()<<endl;
		stateStack.pop();
	}
	fout<<"PD stack:"<<endl;
	for (int i = 0;i<stateStack.size();i++)
	{
		fout<<PDstack.top()<<endl;
		PDstack.pop();
	}
	system("pause");
	exit(0);
}
void PDA::readinTables(string actionfile, string gotofile)
{
	ifstream fin(actionfile);
	string s;
	while(fin>>s)
	{
		Symbol sym(s,1);
		fin>>s;
		int line = atoi(s.c_str());
		fin>>s;
		actionTable[pair<int,Symbol>(line,sym)] = s;
	}
	fin.close();
	s = "";
	fin.open(gotofile);
	while (fin>>s)
	{
		Symbol sym(s,0);
		fin>>s;
		int line = atoi(s.c_str());
		fin>>s;
		gotoTable[pair<int,Symbol>(line,sym)] = atoi(s.c_str());
	}
	fin.close();
}
void PDA::parse()
{
	
	stateStack.push(0);
	while(1)
	{
		if (isError())
			error(1);
		if (sentence[reader].isTermial())
		{
			string s = actionTable[pair<int,Symbol>(stateStack.top(),sentence[reader])];
			if (s == "error" || s == "")
			{
				
				error(0);
			}
			if( s == "acc" && isSuccess())
			{
				success();
				return;
			}
			else
			{
				char c1 = s[0];
				string subs(s,1,s.size()-1); 
				int c2 = atoi(subs.c_str());
				if (c1 == 's')
				{
					stateStack. push(c2);
					shift();
				}
				else
					if (c1 == 'r')
					{
						Symbol L = reduce(rs.rules[c2]);
						cout<<rs.rules[c2];
						for (int i = 0;i<rs.rules[c2].Right.size();i++)
							stateStack.pop();
						stateStack.push( gotoTable[pair<int,Symbol>(stateStack.top(),L)]);
						translation(rs.rules[c2],c2);
					}
					else
						error(3);
			}
		}

	}
}

