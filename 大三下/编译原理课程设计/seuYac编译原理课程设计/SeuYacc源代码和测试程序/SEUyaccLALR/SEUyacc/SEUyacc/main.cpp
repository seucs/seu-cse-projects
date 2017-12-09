#include <iostream>
#include <iomanip>
#include "FileReader.h"
#include "ItemStructure.h"
using namespace std;
ruleSet rs;
CalItemSets pda;
int main()
{
	ofstream fout;
	FileReader fd;
	char* filename = "minic.y";
	fd.ParseYFile(filename);
	rs.calFF();
	rs.printFirst();
	pda.setStartRule(0);
	pda.genPDALALR();
	pda.ReduceAndConflict();
	int SetNum = pda.vItemSet.size();
	cout << pda << endl;

	cout<<"Parsing succeed!"<<endl;

	
	fout.open("./genCode/actionTable");
	for (int i = 0; i < SetNum; i++)
	{
		//cout << setw(4) << i;
		for (Symbol s : rs.Symbols)
		{
			if (s.isTermial())
			{
				if (pda.actionTable[pair<int, Symbol>(i, s)] == "")
					fout<<s._name<<' '<<i<<' '<< "error"<<endl;
				else
					fout<<s._name<<' '<<i<<' '<< pda.actionTable[pair<int, Symbol>(i, s)]<<endl;
			}
			//cout << setw(4) << pda.actionTable[pair<int, Symbol>(i, s)];
		}
		//cout << endl;
		//cout<<endl;
	}
	fout.close();

	
	cout << endl;
	fout.open("./genCode/gotoTable");
	for (int i = 0; i < SetNum; i++)
	{
		//cout << setw(4) << i;
		for (Symbol s : rs.Symbols)
		{
			if (!s.isTermial())
				fout<<s._name<<' '<<i<<' '<< pda.gotoTable[pair<int, Symbol>(i, s)]<<endl;
				//cout << setw(4) << pda.gotoTable[pair<int, Symbol>(i, s)];
		}
		//cout<<endl;
		//cout << endl;
	}
	fout.close();

	fout.open("./genCode/yySeuYacc.cpp");
	fout<<"#include \"PDA.h\""<<endl;
	fout<<"#include <vector>"<<endl;
	fout<<"#include \"yyseuLex.h\""<<endl;
	fout<<"#include \"FileReader.h\""<<endl;
	for (int i = 0;i<fd.preCode.size();i++)
		fout<<fd.preCode[i]<<endl;
	
	fout<<"extern ruleSet rs;"<<endl;
	fout<<"void yyparse()"<<endl;
	fout<<"{"<<endl;
	fout<<"FileReader fd;"<<endl;
	fout<<"fd.ParseYFile(\""<<filename<<"\");"<<endl;
	fout<<"vector<Symbol> s;"<<endl;
	//fout<<"ifstream fin(\"test.c\");"<<endl;
	fout<<"ifstream fin(base);"<<endl;
	fout<<"string str;"<<endl;
	fout<<"while (1)"<<endl;
	fout<<"{"<<endl;
	fout<<"	string t = seuLex(fin);"<<endl;
	fout<<"	if (t == \"ERROR\") break;"<<endl;
	fout<<" if (t != \"SPACE\"){"<<endl;
	fout<<"	Symbol sym(t,1,seuLexLastLex);"<<endl;
	fout<<"	s.push_back(sym);}"<<endl;
	fout<<"}"<<endl;
	fout<<"Symbol sym(\"$\",1);"<<endl;
	fout<<"s.push_back(sym);"<<endl;
	fout<<"fin.close();"<<endl;
	fout<<"PDA pda(rs);"<<endl;
	fout<<"pda.input(s);"<<endl;
	fout<<"pda.readinTables(\"actionTable\",\"gotoTable\");"<<endl;
	fout<<"pda.parse();"<<endl;
	fout<<"}"<<endl;
	for (int i = 0;i<fd.userCode.size();i++)
		fout<<fd.userCode[i]<<endl;

	fout.close();

	fout.open("./genCode/translation.h");
	fout<<"#pragma once"<<endl;
	fout<<"#include \"ItemStructure.h\""<<endl;
	fout<<"#include \"SDT.h\""<<endl;
	fout<<"#include \"quadruple.h\""<<endl;
	fout<<"void translation(rule R,int ruleID)"<<endl;
	fout<<"{"<<endl;
	fout<<"	string temp;"<<endl;
	fout<<"	string sym[20];"<<endl;
	fout<<"	switch (ruleID)"<<endl;
	fout<<"	{"<<endl;
	for (int i = 0;i<fd.semCode.size();i++)
	{
		if (fd.semCode[i]!="")
		{
			fout<<"case "<<i<<" : "<<endl;
			fout<<"for (int i = 0;i<R.Right.size();i++)"<<endl;
			fout<<"{"<<endl;
			fout<<"	sym[i] = SDTStack.top();"<<endl;
			fout<<"	SDTStack.pop();"<<endl;
			fout<<"}"<<endl;
			fout<<fd.semCode[i]<<endl;
			fout<<"break;"<<endl;
		}
	}
	fout<<"default:"<<endl;
	fout<<"break;"<<endl;
	fout<<"}"<<endl;
	fout<<"}"<<endl;

	//for (rule r : rs.rules)
	//	cout << r << endl;
/*	Symbol Sp("S\'", 0);
	Symbol S("S", 0);
	Symbol C("C", 0);

	Symbol d("d", 1);
	Symbol c("c", 1);
	vector<Symbol> r;
	r.push_back(S);
	rule t1(Sp,r);
	r.clear();

	r.push_back(C);
	r.push_back(C);
	rule t2(S, r);
	r.clear();

	r.push_back(c);
	r.push_back(C);
	rule t3(C, r);
	r.clear();

	r.push_back(d);
	rule t4(C, r);
	r.clear();

	rs.addRule(t1);
	rs.addRule(t2);
	rs.addRule(t3);
	rs.addRule(t4);

	rs.calFF();
	
	pda.setStartRule(0);
	pda.genPDA();
	cout << pda<< endl;
*/
	return 0;
}