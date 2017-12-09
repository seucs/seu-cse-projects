#include "FormatRegExp.h"

FormatRegExp::FormatRegExp( const map<string,string> &RDP )
{
	regDefPart = RDP;
}
FormatRegExp::~FormatRegExp()
{
}
//将正规表达式中缀转后缀
string FormatRegExp::postfix( string regExpr )
{
	//返回后缀表达式
	string postFixExpr = "";
	//转化堆栈
	stack<char> opStack;
	//预处理：处理双引号，大括号，中括号，通配符，转义字符；将运算符、小括号转为内码
	regExpr = preProcess(regExpr);
	for ( int i = 0; i < regExpr.length(); i++) 
	{
		char c = regExpr[ i ];
		if ( c == LEFT_LITTLE_BRACKET ) {
			opStack.push(LEFT_LITTLE_BRACKET);
		} else if ( c== STAR ) {
			postFixExpr += STAR;
		} else if ( c== ONE_OR_MORE) {
			postFixExpr += ONE_OR_MORE;
		} else if ( c== ONE_OR_NONE ) {
			postFixExpr += ONE_OR_NONE;
		} else if ( c== RIGHT_LITTLE_BRACKET ) {
			while ( opStack.top() !=  LEFT_LITTLE_BRACKET ) {
				postFixExpr += opStack.top();
				opStack.pop();
			}
			opStack.pop();
		} else if ( c == OR ) {
			while ( !opStack.empty() && opStack.top() != LEFT_LITTLE_BRACKET ) {
				postFixExpr += opStack.top();
				opStack.pop();
			}
			opStack.push(OR);
		}else if ( c == AND ) {
			while (  !opStack.empty() && opStack.top() == AND  ) {
				postFixExpr += opStack.top();
				opStack.pop();
			}
			opStack.push(AND);
		}else {
			postFixExpr += c;
		}
	}
	//全部出栈
	while ( !opStack.empty() )
	{
		postFixExpr += opStack.top();
		opStack.pop();
	}
	return postFixExpr;
}
//预处理：处理双引号，大括号，中括号，通配符，转义字符；将运算符转为内码
string FormatRegExp::preProcess(string s) 
{
	//预处理返回字符串
	string res = "";
	//当前扫描字符串
	char cur = 0;
	//遍历字符串
	for ( int i = 0; i < s.length();  i++ ) 
	{
		cur = s[i];
		//当前为 左中括号 【 
		if ( cur == '[' )
		{
			string bs = "";
			while ( s[++i]!=']' ) {
				if( s[i] == '\\')
					bs += escape( s[ ++i ]);
				else
					bs += s[i];
			}
			res += proBrackets(bs);
		}
		//通配任意字符
		else if ( cur == '.' )
		{
			char c1 = 1;
			char c2 = 127;
			string s = "-";
			s = c1 + s;
			s = s + c2;
			res += proBrackets(s);
		} 
		//当前为转义字符，直接处理为单个字符
		else if ( cur == '\\' )
		{
			res +=  escape( s[ ++i ]);
		} 
		//为大括号，查找正规定义
		else if ( cur == '{' ) 
		{
			string regDef = "";
			while ( s[++i] != '}'  ){
					regDef += s[i];
			}
			//将正规定义处理成简单正规式
			res += proBrackets( regDefPart[regDef].substr(1, regDefPart[regDef].size() - 2) );
		} 
		//双引号中内容按照字符看待
		else if ( cur == '"') 
		{
			while ( s[++i] !='"' )
				res += s[i];
		}else if ( cur==  '*' ) {
			res += STAR;
		} else if ( cur== '+' ) {
			res += ONE_OR_MORE;
		} else if ( cur== '?' ) {
			res += ONE_OR_NONE;
		} else if ( cur == '|'){
			res += OR;
		}else if( cur == '('){
			res += LEFT_LITTLE_BRACKET;
		}else if( cur == ')'){
			res += RIGHT_LITTLE_BRACKET;
		}
		//正常字符
		else {
			res += cur;
		}
	}

	string addAND;
	for( int i = 0; i < res.size() ; i++)
	{
		addAND += res[i];
		//添加连接运算符
		if ( i+1 < res.length() ) {
			char d = res[i+1];
			//当前字符不是二元运算符或左括号，下一个字符不是运算符或右括号
			if (  !isBinOp(res[i])  && res[i]!=LEFT_LITTLE_BRACKET 
				&& d!=RIGHT_LITTLE_BRACKET && !isUniOp(d) && !isBinOp(d))
				addAND += AND;
		}
	}
	res = addAND;
	return res;
}
//处理中括号，将中扩号中内容用  |  运算符连接起来，并用 （）表示优先运算
string FormatRegExp::proBrackets( string s )
{
	//返回串
	string res = "";
	res += LEFT_LITTLE_BRACKET;
	//上一个扫描字符
	char last = 0;
	//当前扫描字符
	char cur;

	//遍历字符串
	for ( int i=0; i<s.length(); i++ )
	{
		cur = s[i];
		//如果以 - 连接表示
		if ( cur == '-' )
		{
			cur = s[i+1];
			i++;
			//从上一个字符到当前字符
			for ( int x = last+1; x <= cur; x++ )
			{
				////如果是运算符和括号
				res += x;
				//如果不是最后一个字符
				if ( x < cur ) 
					res += OR;
			}
		} 
		//如果是正常字符
		else 
		{
			res += cur;
			last = cur;
		}
		//用或运算符连接
		if ( i+1<s.length() ) {
			res += OR;
		}
	}
	//返回
	res += RIGHT_LITTLE_BRACKET;
	return res;
}
//处理转义字符
char FormatRegExp::escape( char d ) {
	if ( d == 'n' ) {
		return '\n';
	} else if ( d=='r' ) {
		return '\r';
	} else if ( d == 't' ) {
		return '\t';
	} else {
		return d;
	}
}
//一元运算符
bool FormatRegExp::isUniOp(char c) {
	if ( c == STAR)
		return true;
	else if ( c ==ONE_OR_MORE )
		return true;
	else if ( c ==ONE_OR_NONE )
		return true;
	else
		return false;
}
//二元运算符
bool FormatRegExp::isBinOp(char c) {
	if ( c == OR)
		return true;
	else 
		return false;
}