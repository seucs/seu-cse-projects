#include "CodeGenerator.h"

//打开文件，并包含头文件
CodeGenerator::CodeGenerator(string fileName)
{
	//打开文件并写入要包含的头文件
	File.open(fileName, fstream::out | ios_base::trunc);
	File<< "#include <"<< "iostream" <<  ">\n" ;
	File<< "#include <"<< "deque" <<  ">\n" ;
	File<< "#include <"<< "cassert" <<  ">\n" ;
	File<< "#include <"<< "string" <<  ">\n" ;
}
	CodeGenerator::~CodeGenerator()
	{		
		File.close();
	}

//根据规则，生成驱动程序
void CodeGenerator::genDriver(vector<string> rules, int enterState)
{
	writeln("string seuLexLastLex;");
	writeln("int seuLex(std::istream& cin = std::cin, std::ostream& cout = std::cout)");
	writeln("{");
	writeln("while ( true )");
	writeln("{");
	File<< "int currentState = " <<enterState<<";\n";
	writeln("int matchedState = 0;");
	writeln("int currentLength = 0;");
	writeln("int matchedLength = 0;");
	writeln("seuLexLastLex.clear();");
	writeln("char c;");
	writeln("std::deque<char> q;");
	writeln("while ( currentState!=-1 && cin.get(c) )");
	writeln("{");
	writeln("q.push_back(c);");
	writeln("currentLength++;");
	writeln("currentState = TABLE[currentState][c];");
	writeln("if ( STATE_PATTERN[currentState] != -1 )");
	writeln("{");
	writeln("matchedState = currentState;");
	writeln("matchedLength = currentLength;");
	writeln("}");
	writeln("}");
	writeln("if ( matchedLength>0 )");
	writeln("{");
	writeln("while ( currentLength>matchedLength )");
	writeln("{");
	writeln("cin.putback(q.back());");
	writeln("q.pop_back();");
	writeln("currentLength--;");
	writeln("}");
	writeln("while ( !q.empty() )");
	writeln("{");
	writeln("seuLexLastLex += q.front();");
	writeln("q.pop_front();");
	writeln("}");
	writeln("switch ( STATE_PATTERN[matchedState] )");
	writeln("{");

	for (int i = 0; i < rules.size(); i++)
		File<<"case "<< i << ":\n" << rules[i] << "\nbreak;\n";

	writeln("default:");
	writeln("assert(false);");
	//writeln("}");
	writeln("}");
	writeln("}");
	writeln("else ");
	writeln("{");
	writeln("return -1;");
	writeln("}");
	writeln("}");
	writeln("return 0;");
	writeln("}");
}

//将二维vector转化为二维常量数组
void CodeGenerator::genTable( vector<vector<int> > t, string name)
{
	File<<"const int "<<name<<"[" <<t.size()<< "][" << t[0].size() << "] = {\n";
	for ( int i=0; i<t.size(); i++ )
	{
		File<<'\t';
		for ( int j = 0; j < t[i].size(); j++ )
			File<<t[i][j]<<',';
		File<<'\n';
	}
	File<<"};\n";
}
//将vector转化为常量数组
void CodeGenerator::genVector(vector<int> v,string name)
{
	File<<"const int "<<name<<"[" <<v.size()<< "] = {\n";
	for ( int i=0; i<v.size(); i++ )
		File<<v[i]<<",";
	File<<"};\n";
}
//写文件并换行
void CodeGenerator::writeln(string str)
{
	write(str+"\n");
}
//写文件
void CodeGenerator::write(string str)
{
	File<<str;
}