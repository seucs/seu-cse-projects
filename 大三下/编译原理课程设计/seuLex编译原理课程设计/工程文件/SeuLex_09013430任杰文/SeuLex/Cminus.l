%{
#include <fstream>
using namespace std;
typedef enum
{
	INT = 1, 
	FLOAT,
	
	ID,
	STRUCT,
	IF,
	ELSE,
	RETURN,
	NUM,

	LPAREN,
	RPAREN,
	LCOMMENT,
	RCOMMENT,
	LBRACK,
	RBRACK,

	ASSIGN,
	SEMI,
	COMMA,
	
	MINUS,
	PLUS,
	TIMES,
	OVER,
	MOD,
	EQ,

	CHAR,
	BOOL,
	DOUBLE,
	VOID,

	_NULL,
	TRUE,
	FLASE,
	WHILE,

	PLUSASSIGN,
	MINUSASSIGN,
	TIMESASSIGN,
	OVERASSIGN,
	MODASSIGN,

	LOGICNOT,
	LOGICAND,
	LOGICOR,

	NEQ,
	LT,
	GT,
	LEQ,
	GEQ,

	NL,
	SPACE,
	ERROR
} TokenType;
int lineno = 1;
%}

alpha			[A-Za-z]
digit			[0-9]
alphanum		[A-Za-z0-9]

%%

"int"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return INT;
"char"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return CHAR;
"bool"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return BOOL;
"float"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return FLOAT;
"double"		cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return DOUBLE;
"void"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tPrimitive Type\n";return VOID;

"NULL"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tInternal Constant\n";return _NULL;
"true"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tInternal Constant\n";return TRUE;
"false"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tInternal Constant\n";return FLASE;

"if"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tFlow Controller\n";return IF;
"else"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tFlow Controller\n";return ELSE;
"while"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tFlow Controller\n";return WHILE;
"return"		cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tFlow Controller\n";return RETURN;

{alpha}{alphanum}*	cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tIdentifier\n";return ID;
{digit}+("."{digit}+)?	cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tNumber\n";return NUM;

"="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return ASSIGN;
"+="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return PLUSASSIGN;
"-="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return MINUSASSIGN;
"*="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return TIMESASSIGN;
"/="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return OVERASSIGN;
"%="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tAssign Operator\n";return MODASSIGN;

"-"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tArithmetic Operator\n";return MINUS;
"+"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tArithmetic Operator\n";return PLUS;
"*"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tArithmetic Operator\n";return TIMES;
"/"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tArithmetic Operator\n";return OVER;
"%"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tArithmetic Operator\n";return MOD;

"!"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tLogical Operator\n";return LOGICNOT;
"&&"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tLogical Operator\n";return LOGICAND;
"||"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tLogical Operator\n";return LOGICOR;

"=="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return EQ;
"<"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return NEQ;
">"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return LT;
"<="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return GT;
">="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return LEQ;
"!="			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tRelation Operator\n";return GEQ;

","			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return COMMA;
";"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return SEMI;
"{"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return LCOMMENT;
"}"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return RCOMMENT;
"("			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return LPAREN;
")"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return RPAREN;
"["			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return LBRACK;
"]"			cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tSeparating Character\n";return RBRACK;

\n			lineno++;return NL;
[" "\t\r]			;/*ignore white space*/ return SPACE;
.			  cout<<seuLexLastLex<<"\t at line "<<lineno<<"\t----\tBad Character\n"; return -1; return ERROR; 
			
			

%%

int main()
{
	ifstream in("demo.cpp");
	while(  seuLex( in ) != -1 );
	cout<<"Lexing Ended\n";
	system("pause");
	return 0;
}
