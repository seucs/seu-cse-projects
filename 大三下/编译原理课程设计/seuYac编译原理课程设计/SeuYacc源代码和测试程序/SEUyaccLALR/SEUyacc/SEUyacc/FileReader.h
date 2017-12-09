#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <vector>
#include "ItemStructure.h"
enum yaccState
{
	NORMAL
	, ERROR
	, EXT
	, TOKEN
	, TYPE
	, UNION
	, LEFT
	, NONASSOC
	, RIGHT
};

class FileReader
{
public:

	FileReader();
	~FileReader();
	int ParseYFile(char* fileName);
public:
	//分析定义部分代码
	void getDefPart();
	//分析规则部分
	void getRegPart();
	//分析子程序部分
	void getCodePart();
	yaccState getState(std::string str);
	std::ifstream fin;
	std::map<std::string,yaccState> specSym;
	int lineNo;
	std::vector<std::string> preCode;
	std::vector<std::string> unionCode;
	std::vector<std::string> userCode;
	std::vector<std::string> semCode;
};

