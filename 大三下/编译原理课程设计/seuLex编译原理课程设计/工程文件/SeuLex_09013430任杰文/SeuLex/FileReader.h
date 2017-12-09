#ifndef LEXFILEREADER_H
#define LEXFILEREADER_H
#include "headers.h"

//Lex输入文件的解析
class FileReader
{
public:
	FileReader(string fileName);
	~FileReader();
public:
	//返回C语言定义部分代码
	vector<string> getDefPart();
	//返回正规定义段
	map<string,string> getRegDefPart();
	//返回正规表达式
	vector<string> getRegularExpression();
	//返回正规表达式后对应代码
	vector<string> getCode();
	//返回用户定义子例程
	vector<string> getLastPart();
private:
	// Cminus.l 文件流
	ifstream fcin;
	//C语言定义部分的行数
	int definePartNum;
	// 正则表达式的行数
	int regularExpressionNum;
	// 用户子例程段的代码的行数
	int lastPartNum;
};

#endif
