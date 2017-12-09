#ifndef INFIXTOPOSTFIX_H
#define INFIXTOPOSTFIX_H
#include "headers.h"

//标准化正则表达式
class FormatRegExp
{
public:
	FormatRegExp( const map<string,string> &RDP );
	~FormatRegExp();
	//将正规表达式中缀转后缀
	string postfix( string regExpr );
private:
	//预处理：处理双引号，大括号，中括号，通配符，转义字符："{}[].\    ;  并将运算符转为内码：只包含内码表示的 （）+*？| & 
	string preProcess(string s);
	//处理中括号，将中扩号中内容用  |  运算符连接起来，并用 （）表示优先运算
	string proBrackets( string s );
	//处理转义字符
	char escape( char d );
	//一元运算符
	bool isUniOp(char c);
	//二元运算符
	bool isBinOp(char c);
private:
	//cminus.l 正规定义
	map<string,string> regDefPart;
};
#endif