#include "headers.h"
#include "FileReader.h"
#include "FormatRegExp.h"
#include "NFA.h"
#include "MergedNFA.h"
#include "DFA.h"
#include "CodeGenerator.h"

int main()
{
	//读取lex输入文件
	string lexFile = "Cminus.l";

	//将cminus.l 文件分割为C 语言定义段，正规定义段，规则段（返回正规式的数组与 C 语句的数组，下标对应），用户子程序段
	FileReader lexFileReader(lexFile);
	//语言定义段
	vector<string> defPart = lexFileReader.getDefPart();
	//正规定义段
	map<string,string> regDefPart = lexFileReader.getRegDefPart();
	//规则段
	vector<string> regularPart = lexFileReader.getRegularExpression();
	vector<string> regularPartCode = lexFileReader.getCode();
	int numOfRegularPart = regularPart.size();
	//用户子程序段
	vector<string> demoOfUser = lexFileReader.getLastPart();

	//对每个正规表达式构造NFA
	cout<<"对每个正规表达式构造NFA："<<endl;
	vector<NFA*> nfas( numOfRegularPart );
	for ( int i = 0; i <numOfRegularPart;  i++ ) 
	{
		nfas[i] = new NFA(regularPart[i], regDefPart);
		cout<<setw(25)<<left<<regularPart[i].c_str()<<"   对应NFA含有："<<nfas[i]->numStateOfNFA<<"个状态；"<<endl;
	}
	//合并NFA
	cout<<"合并NFA："<<endl;
	MergedNFA mn(nfas);
	cout<<"NFA合并完成，当前NFA有："<<mn.numStateOfNFA<<"个状态"<<endl;

	//构造DFA
	cout<<"构造NFA："<<endl;
	DFA dfa(&mn);
	cout<<"构造NFA完成，当前DFA有："<<dfa.getNumStates()<<"个状态"<<endl;
	cout<<"最小化NFA："<<endl;
	dfa.minimize();
	cout<<"最小化NFA完成，当前DFA有："<<dfa.getNumStates()<<"个状态"<<endl;

	//生成DFA的状态转移表及其驱动程序
	cout<<"生成词法分析程序 yySeuLex.cpp："<<endl;
	CodeGenerator codeGenerator("yySeuLex.cpp");
	//生成C语言定义段
	for( int i = 0; i < defPart.size(); i++)
		codeGenerator.writeln( defPart[i] );
	//生成DFA状态转化表
	codeGenerator.genTable(*dfa.getTable(), "TABLE");
	//生成DFA识别token查找表
	codeGenerator.genVector(*dfa.getStatePattern(), "STATE_PATTERN");
	//生成驱动程序
	codeGenerator.genDriver(regularPartCode,dfa.getEnterState());
	//生成用户子例程段
	for(int i=0; i<demoOfUser.size(); i++)
		codeGenerator.writeln(demoOfUser[i]);
	cout<<"词法分析程序 yySeuLex.cpp生成成功！"<<endl;

	system("pause");
	return 0;
}