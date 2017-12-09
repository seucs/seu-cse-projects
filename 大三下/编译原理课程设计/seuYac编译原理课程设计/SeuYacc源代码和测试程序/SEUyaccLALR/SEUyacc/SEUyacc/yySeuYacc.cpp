#include "PDA.h"
#include <vector>
#include "yyseuLex.h"
#include "FileReader.h"
/*	minic.y(1.9)	17:46:21	97/12/10
*
*	Parser demo of simple symbol table management and type checking.
*/
#include	<stdio.h>	/* for (f)printf() */
#include	<stdlib.h>	/* for exit() */
extern int	line	= 1;	/* number of current source line */
extern int	seulex();	/* lexical analyzer generated from lex.l */
char	*yytext;	/* last token, defined in lex.l  */		/* current symbol table, initialized in lex.l */
char		*base;		/* basename of command line argument */
void
yyerror(char *s)
{
fprintf(stderr,"Syntax error on line #%d: %s\n",line,s);
fprintf(stderr,"Last token was \"%s\"\n",yytext);
exit(1);
}
extern ruleSet rs;
void yyparse()
{
FileReader fd;
fd.ParseYFile("minic.y");
vector<Symbol> s;
ifstream fin(base);
string str;
while (1)
{
	string t = seuLex(fin);
	if (t == "ERROR") break;
 if (t != "SPACE"){
	Symbol sym(t,1,seuLexLastLex);
	s.push_back(sym);}
}
Symbol sym("$",1);
s.push_back(sym);
fin.close();
PDA pda(rs);
pda.input(s);
pda.readinTables("actionTable.txt","gotoTable.txt");
pda.parse();
}


int
main(int argc,char *argv[])
{
base = "test.c";
yyparse();
}
