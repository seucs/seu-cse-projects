#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H
#include "headers.h"

//生成DFA 的状态转移表及其驱动程序
class CodeGenerator
{
public:
	//打开文件，并包含头文件
	CodeGenerator(string fileName);
	~CodeGenerator();
	//根据规则，生成驱动程序,默认入口状态为0
	void genDriver( vector<string> rules,int enterState = 0 );
	//将二维vector转化为二维常量数组
	void genTable(vector<vector<int> > t,string);
	//将vector转化为常量数组
	void genVector(vector<int> v,string);
	//写文件并换行
	void writeln(string);
	//写文件
	void write(string);

private:
	fstream File;
};

#endif