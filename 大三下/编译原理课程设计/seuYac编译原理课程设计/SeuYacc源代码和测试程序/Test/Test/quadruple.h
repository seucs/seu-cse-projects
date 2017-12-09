#pragma once
#include <string>
#include <vector>
#include <stack>
#include <fstream>
using namespace std;
class quadurple
{
public:
	string sym;		//the symbol of the operation
	string result;   
	string i;      //third element of a quadurple
	string j;	   //fourth element of a quadurple
	quadurple(string SYM,string RESULT,string I,string J);
	friend ostream& operator << (ostream& out, quadurple);
};

