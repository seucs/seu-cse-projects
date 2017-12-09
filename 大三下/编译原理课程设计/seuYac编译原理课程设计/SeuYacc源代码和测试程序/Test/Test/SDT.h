#pragma once
#include "quadruple.h"
stack<string> SDTStack;
stack<int> TC;
stack<int> FC;
vector<quadurple> quadSet;
int tempID = 0;
int lineo = 0;
ofstream fout("IntermediaCode.c");
string intToString(int a)
{
	char c[100] =  "";
	itoa(a,c,10);
	string C(c);
	return c;
}
string newTemp()	//produce a new temp variable of Tx
{
	char id[20] = "";
	tempID++;
	itoa(tempID,id,10);
	string ID(id);
	return "T"+ID;
}
void gen(string sym,string result,string i, string j) //generate a new quadruple
{
	
	quadurple q(sym,result,i,j);
	quadSet.push_back(q);
	lineo++;
}
void quadOutput()
{
	for (int i = 0;i<quadSet.size();i++)
	{
		fout<<i<<" : ";
		fout<<quadSet[i];
	}
}
