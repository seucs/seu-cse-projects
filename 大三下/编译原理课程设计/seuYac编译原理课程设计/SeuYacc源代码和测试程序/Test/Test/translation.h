#pragma once
#include "ItemStructure.h"
#include "SDT.h"
#include "quadruple.h"
void translation(rule R,int ruleID)
{
	string temp;
	string sym[20];
	switch (ruleID)
	{
case 24 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
gen("j"+sym[2],sym[1],sym[3],"0");TC.push(lineo);FC.push(lineo+1);gen("j","_","_","0");quadSet[TC.top()-1].j=intToString(lineo);TC.pop();
break;
case 25 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
gen("j","_","_","0");TC.push(lineo);quadSet[FC.top()-1].j=intToString(lineo);FC.pop();
break;
case 27 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
quadSet[TC.top()-1].j=intToString(lineo);TC.pop();
break;
case 28 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
gen(sym[1],sym[2],"_",sym[0]);
break;
case 36 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
temp=newTemp();gen(sym[1],temp,sym[0],sym[2]);SDTStack.push(temp);
break;
case 37 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
temp=newTemp();gen(sym[1],temp,sym[0],sym[2]);SDTStack.push(temp);
break;
case 38 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
temp=newTemp();gen(sym[1],temp,sym[0],sym[2]);SDTStack.push(temp);
break;
case 39 : 
for (int i = 0;i<R.Right.size();i++)
{
	sym[i] = SDTStack.top();
	SDTStack.pop();
}
temp=newTemp();gen(sym[1],temp,sym[0],sym[2]);SDTStack.push(temp);
break;
default:
break;
}
}
