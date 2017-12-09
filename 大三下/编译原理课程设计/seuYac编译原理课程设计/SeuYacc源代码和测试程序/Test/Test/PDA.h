#pragma once
#include "ItemStructure.h"
#include <fstream>
class PDA
{
public:
	PDA();
	PDA(ruleSet RS);
	PDA(vector<rule> RS);
	ruleSet rs;
	vector<Symbol> sentence; //Sentence waiting to be parsed
	int reader;				//where the PDA is reading
	stack<int> stateStack;  
	stack<Symbol> PDstack;
	vector<Symbol> stackData;
	map<pair<int, Symbol>, string> actionTable;
	map<pair<int, Symbol>, int> gotoTable;
public:
	void readinTables(string actionfile, string gotofile);
	void parse();			//true yyparse()
	void input(vector<Symbol> s);
	void shift();			
	Symbol reduce(rule r);	//reduce with a given rule
	bool isSuccess();
	bool isError();
	void success();
	void error(int);
};