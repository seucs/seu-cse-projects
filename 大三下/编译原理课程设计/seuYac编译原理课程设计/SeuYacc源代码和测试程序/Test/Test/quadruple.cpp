#include "quadruple.h"
quadurple::quadurple(string SYM,string RESULT,string I, string J)
{
	sym = SYM;
	result = RESULT;
	i = I;
	j = J;
}
ostream & operator<<(ostream & out, quadurple q)
{
	out<<"("<<q.sym<<" , "<<q.result<<" , "<<q.i<<" , "<<q.j<<")"<<endl;
	return out;
}