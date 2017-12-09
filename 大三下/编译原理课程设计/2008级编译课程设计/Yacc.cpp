/////////////////////////////////////////////////////////
//编译原理实验——第二部分:YACC                        //
//作者:杨俊                                            //
//时间:2006年4~5月                                     //
/////////////////////////////////////////////////////////
#include<iostream>
#include<iomanip>
#include<fstream>
#include<string>
#include<set>
#include<vector>
#include<stack>
#include<queue>
#include<hash_set>
#include<hash_map>
#include<map>
#include<set>
#include<conio.h>
#define ACCEPT 0
#define ERROR 40000
#define EPSLONG 39999
#define SEGMENT_DELIM 39998
#define DEF_SEG_BEGIN 39997
#define DEF_SEG_END 39996
#define TOKEN 39995
#define TYPE 39994
#define UNION 39993
#define LEFT 39992
#define TERMINATE 39991
#define NOMATCH 39000
#define RIGHT 38999
#define SEGMENT_REG 38998
#define SELF_DEF 38997
#define NONTERMINS 35000
#define TERMINS 30000
using namespace std;
struct Producer
{
	int left;
	vector<int> right;
};
ifstream fin;
ofstream fout;
ofstream sout;
ofstream hout;
char NO=1;
//一些常量的定义
vector<Producer> producerSet;//存储所有产生式的向量，在读入产生式时进行初始化！
hash_map<int,vector<int> > hpSet;//以产生式的左部为关键字，以对应产生式的编号为内容的哈希表。
hash_map<int,int> terminSet;//存储终结符,***并且要在其中加入结束符号#
hash_map<int,int> nonterminSet;//存储非终结符,并同时存储一个到action表头的对应关系
vector<vector<int> > actionTable;//存储动作表，其中第二维大小必须已知。
//目前打算人为规定，产生的非终态符的符号的数值范围。
hash_map<string,int> tsymTable;//这个表是终结符的串到相应数字编码的映射
hash_map<string,int> ntsymTable;
hash_map<int,int> precedenceTable;//优先级表
vector<int> producerPreTable;//产生式的优先级表，和上面的优先级表联合使用
hash_set<int> leftTable;//左结合表
hash_set<int> rightTable;//右结合表
hash_map<int,string> symclaTable;//存储语义值类型的表
vector<string> produceActionTable;//存储语义动作的表
//class ItemLess;//函数对象Item的小于算子
//关于类Item的定义
class Item
{
public:
	//friend class ItemLess;
	friend bool operator <(const Item &it1,const Item &it2);
	Item(int producerNum,int currentPos);
	int getCurrSym();//返回下一个将要移进的符号，如果已经到达最后，则返回-1，表示可归约。
	int getNextSym() const;//返回再一下要移进的符号，在求闭包运算中计算搜索符时使用。
	int getProdN();//返回产生式的编号。
	bool isEnd();//是否右部已经全部移进。
	bool nextIsEnd() const;//右部是否到达最后一个符号，求闭包运算时用。
	void setSearchSym(const hash_set<int> &searchSymbol);
	const hash_set<int> &getSearchSym() const;
	void move();
private:
	
	int pn;//存储产生式的编号。
	int pos;//存储此项目的移进位置。
	hash_set<int> searchSym;//存储此项目的搜索符，作为公共可以让外部函数first直接使用
};



//关于类Item的具体函数定义如下
//........
Item::Item(int producerNum,int currentPos)
{
	this->pn=producerNum;
	this->pos=currentPos;
}
int Item::getCurrSym()
{
	return producerSet[pn].right[pos];//返回产生式右部下一移进符号
}
int Item::getNextSym() const
{
	return producerSet[pn].right[pos+1];
}
int Item::getProdN()
{
	return this->pn;
}
bool Item::isEnd()
{
	if(producerSet[pn].right[0]==EPSLONG)
		return true;
	if(this->pos==producerSet[pn].right.size())
		return true;
	return false;
}
bool Item::nextIsEnd() const
{
	if(this->pos==producerSet[pn].right.size()-1)
		return true;
	return false;
}
void Item::setSearchSym(const hash_set<int> &searchSymbol)
{
	searchSym=searchSymbol;
}

const hash_set<int> &Item::getSearchSym() const
{
	return searchSym;
}

void Item::move()
{
	pos++;
	return;
}


bool operator <(const Item &it1,const Item &it2)
{//此处的<算子定义很关键，一定要满足偏序性
	if(it1.pn==it2.pn)
	{
		if(it1.pos==it2.pos)
			return it1.getSearchSym()<it2.getSearchSym();
		else return it1.pos<it2.pos;
	}
	else return it1.pn<it2.pn;
}
//

//函数的声明
void closure(set<Item> &itemSet);
//下面是求first的函数,参数为产生式编号，first符号集，已出现非终结符号集
//此为递归函数，为外部不可调用
void first(const Item &item,hash_set<int> &searchSymbol);
//下面是外部可调的first函数
bool getfirstSet(int producerNum, hash_set<int> &firstSet, hash_set<int> &isUsed);
bool produce();//产生项目集之间的转换关系
void generateTableCode();
void generateParseCode();
void generateSemanticActionCode();//生成语义动作函数
void generateMainCode();
void generateHead();
void generateSVCode();//产生给终结符赋默认值 的代码:void getvalue();
int readInputFile();//前段输入部分的主要函数，用于读入文件并处理
int specSymParse();
bool readReg();//规则段的扫描程序
bool readAproducer();//读取一个产生式
void genhpSet();
void outSignalTable();
void displayI(Item & item);
////////////////////////////////////////////////////////////////////
//主函数　　
///////////////////////////////////////////////////////////////////
void main() 
{
	string inputname;
	cout<<"Please input the file name:\n";
	cin>>inputname;
	fin.open(inputname.c_str());

	//fin.open("test2.txt");
	fout.open("yyparse.cpp");
	sout.open("signal_table.txt");//生成符号表的头文件
	hout.open("yytab.h");//生成全局符号语义值变量的类型
	if(fout.fail()||fin.fail()||sout.fail()||hout.fail())
	{
		cout<<"Cannot open output file!"<<endl;
		return;
	}
	//初始化
	Producer ap;
	producerSet.push_back(ap);//首先把拓展方法开始符号放入
	readInputFile();
	generateMainCode();
	outSignalTable();
	cout<<"Finish!\n";
}
void outSignalTable()
{
	hout<<"\n#ifndef YYTAB_H\n";
	hout<<"#define YYTAB_H\n";
	sout<<setw(20)<<"SIGNAL"<<setw(20)<<"VALUE"<<endl;
	sout<<setfill('=')<<setw(41)<<" "<<setfill(' ')<<endl;
	int num=0;
	for(hash_map<string,int>::iterator pi=tsymTable.begin();pi!=tsymTable.end();pi++)
	{
		string delim("~!@#$%^&*()-=+`{}[]\|'\":;/?.,<>");
		int x=pi->first.find_first_of(delim);
		if(x<0||x>=pi->first.size())
		{
			hout<<"#define"<<setw(20)<<pi->first<<setw(20)<<pi->second<<endl;
		}
		sout<<setw(20)<<pi->first<<setw(20)<<pi->second<<endl;
		num++;
	}
	sout<<setfill('=')<<setw(41)<<" "<<setfill(' ')<<endl;
	sout<<setw(20)<<"TOTAL:"<<num<<endl;
	hout<<"#endif\n";
}


void closure(set<Item> &itemSet)
{
	queue<Item> Q;
	//首先要把当前项目集中所有状态放入到队列中。(除去不会产生新项的项)
	for(set<Item>::iterator ps=itemSet.begin();ps!=itemSet.end();ps++)
	{
		//cout<<"in closure "<<ps->getCurrSym()<<endl;
		if(!ps->isEnd()&&nonterminSet.count(ps->getCurrSym()))
			Q.push(*ps);
	}
	//进入循环判断。
	hash_set<int> isU;
	hash_set<int> isE;
	while(!Q.empty())
	{
		Item citem=Q.front();
		Q.pop();
		vector<int> vpi=hpSet[citem.getCurrSym()];
		hash_set<int> searchSym;
		first(citem,searchSym);//此步完成搜索符的计算	
		for(int i=0;i<vpi.size();i++)
		{
			Item tp(vpi[i],0);
			tp.setSearchSym(searchSym);
			if(!itemSet.count(tp))//如果此项目不在项目集中，则需将它加入到项目集中去。
			{                //同时需要进行相应的搜索符的计算。
				itemSet.insert(tp); 
				if(nonterminSet.count(tp.getCurrSym()))
				{
					Q.push(tp);
				}
			}
		}
	}
}

void first(const Item &item,hash_set<int> &searchSymbol)
{
	if(item.nextIsEnd())
	{
		searchSymbol=item.getSearchSym();
	}
	else 
	{
		int sym=item.getNextSym();
		if(terminSet.count(sym))
			searchSymbol.insert(sym);
		else
		{
			vector<int> tv=hpSet[sym];
			hash_set<int> isused;
			for(int i=0;i<tv.size();i++)
			{
				getfirstSet(tv[i],searchSymbol,isused);
			}
		}
	}
}
bool getfirstSet(int producerNum, hash_set<int> &firstSet, hash_set<int> &isUsed)
{
	if ( producerSet.at(producerNum).right[0] == EPSLONG )
		return 0;
	else {
		int i = 0;
		if ( terminSet.count ( producerSet.at(producerNum).right[i] ) )
			firstSet.insert( producerSet.at(producerNum).right[i] );
		else if ( nonterminSet.count ( producerSet.at(producerNum).right[i] ) )
			{
			if ( !isUsed.count( producerSet.at(producerNum).right[i]) )
				{
				isUsed.insert ( producerSet.at(producerNum).right[i] );
				vector < int > tv;
				tv = hpSet[ producerSet.at(producerNum).right[i] ] ;
				vector < int >::const_iterator p1;
				int c = 1;  // 
				for ( p1 = tv.begin(); p1 != tv.end(); p1++ )
				{  
					if(!getfirstSet ( *p1, firstSet, isUsed ))
						{
						c=0;
						}
				}
				while( c == 0 ) 
					{
					i++;
					if ( producerSet.at(producerNum).right[i]!=0 &&
						 nonterminSet.count ( producerSet.at(producerNum).right[i]) &&
						 isUsed.count ( producerSet.at(producerNum).right[i] ) == 0)
						 {
						vector < int > tv2;
						tv2 = hpSet [ producerSet.at(producerNum).right[i] ] ;
						vector < int >::const_iterator p2;
						for (p2 = tv2.begin(); p2 != tv2.end(); p2++ )
							if(!getfirstSet ( *p2, firstSet,isUsed ))
								c=0;
					}
					else if ( terminSet.count ( producerSet.at(producerNum).right[i] ) )
						firstSet.insert ( producerSet.at(producerNum).right[i] );
				}
			}
		}
		return 1;
	}
}

void display(set<Item> & i0)
{
	cout<<"\n----------------ItemSet ---------------------"<<endl;
	for(set<Item>::iterator pi=i0.begin();pi!=i0.end();pi++)
	{
		cout<<pi->getProdN()<<"--"<<pi->getCurrSym()<<"  SEARCH:";
		for(hash_set<int>::const_iterator i=pi->getSearchSym().begin();i!=pi->getSearchSym().end();i++)
			cout<<*i<<" ";
		cout<<"||"<<endl;
	}
	cout<<"\n------------------------\n";
}

void displayI(Item & item)
{
	cout<<"\nitem -----------------------------\n";
	cout<<"PID:"<<item.getProdN()<<endl;
	cout<<"LEFT:"<<producerSet[item.getProdN()].left<<endl;
	cout<<"RIGHT:";
	for(int i=0;i<producerSet[item.getProdN()].right.size();i++)
	{
		cout<<producerSet[item.getProdN()].right[i]<<"  ";
	}
	cout<<"\nSEARCH:";
	for(hash_set<int>::const_iterator i=item.getSearchSym().begin();i!=item.getSearchSym().end();i++)
	{
		cout<<*i<<"  ";
	}
	cout<<"\n-----------------------------------\n";
}
void genhpSet()
{
	for(int i=1;i<producerSet.size();i++)//0号产生式不用管
	{
		if(!hpSet.count(producerSet[i].left))
		{
			pair<int,vector<int> > tp;
			tp.first=producerSet[i].left;
			vector<int> vt;
			tp.second=vt;
			hpSet.insert(tp);
		}
		hpSet[producerSet[i].left].push_back(i);
	}
}
bool produce()
{
	/**
	 *  此处考虑图的表示方法为二维数组，因为在输出时也是以二维数组的形式。
	 *  在此数组中，用正数表示要跳转的状态号，用负数表示某一待归约的产生式号。
	 *  DONE!
	 */
	//首先是一组要用到的常量定义
	int curState=0;//存储项目集的状态号。随项目集的产生动态更新
	int widgeN=nonterminSet.size()+terminSet.size();//表示所有可能的项目集的边
	queue<set<Item> > Q;
	map<set<Item>,int> itemsetTable;//从项目集到某一数字标识的映射
	hash_map<int, set<Item> > moveItemset;//以边为关键字，以进行分类的项目的项目集为内容。
	set<Item> I0;

	Item firstitem(0,0);//定义初始项目集中的项目。
	hash_set<int> searchs;
	searchs.insert(NONTERMINS-1);//表示结束状态
	firstitem.setSearchSym(searchs);

	I0.insert(firstitem);

	firstitem.move();//修改其移进位，因为在后面输出正确状态时要用

	closure(I0);
	Q.push(I0);

	display(I0);
	pair<set<Item>,int> te;//将项目集0放入到项目集的表中
	te.first=I0;
	te.second=curState;
	itemsetTable.insert(te);

	while(!Q.empty())
	{
		set<Item> itemset=Q.front();
		Q.pop();//从队列中取得一个项目集。
		moveItemset.clear();//清空，以进行新一轮循环

		vector<int> f(widgeN);//数组第二维的大小必须预先定义好
		for(int i=0;i<f.size();i++)
		{
			f[i]=ERROR;//以ERROR值对vector进行初始化。
		}

		actionTable.push_back(f);

		int column,row;
		row=itemsetTable[itemset];
		cout<<"\n===================================================================\n";
		//display(itemset);
		cout<<"state = "<<row<<endl;
		cout<<"\n===================================================================\n";

		//开始求它的所有项目的move后的项目集。te
		for(set<Item>::iterator ps=itemset.begin();ps!=itemset.end();ps++)
		{
			if(ps->isEnd())//出现可归约项目，需进行处理。
			{
				for(hash_set<int>::const_iterator pi=ps->getSearchSym().begin();pi!=ps->getSearchSym().end();pi++)
				{
					//对可归约项目，对其中每个搜索符都填入表中
					column=terminSet[*pi];
					if(actionTable[row][column]<=0&&ps->getProdN()!=actionTable[row][column]*(-1))
					{
						cout<<"Reduction & Reduction Confliction"<<endl;
						cout<<"row="<<row<<" column="<<column<<" old="<<actionTable[row][column]
					       <<" new="<<ps->getProdN()<<endl;
						return false;
					}
					actionTable[row][column]=ps->getProdN()*(-1);//变成负数以表示此为归约项,如果为0用于表示accept
					//cout<<"actionTable row="<<row<<" column="<<column<<endl;
				}
			}
			else
			{
				Item tp=*ps;
				tp.move();
				moveItemset[ps->getCurrSym()].insert(tp);
			}
		}
		//对所有新产生的项目集求闭包并判断是否需要加入到队列中去。
		for(hash_map<int,set<Item> >::iterator ph=moveItemset.begin();ph!=moveItemset.end();ph++)
		{
			if(curState==75)
			{
				cout<<"wa";
			}
			closure(ph->second);

			if(!itemsetTable.count(ph->second))
			{
				pair<set<Item>,int> tp;
				tp.first=ph->second;			
				curState++;//只在此处增加项目集，故只在此处修改其值
				tp.second=curState;

				itemsetTable.insert(tp);
				//如果当前的项目集未出现过的话，需要将其放入队列中等待处理。
				Q.push(ph->second);
			}	

			//下面开始生成对应的action表
			//首先求得边所对应的表中的序号
			column=(nonterminSet.count(ph->first)?nonterminSet[ph->first]:terminSet[ph->first]);

			//此处加一些条件，判断是否有归约、移进冲突
			if(actionTable[row][column]<=0)
			{	
				if(producerPreTable[actionTable[row][column]*(-1)]<precedenceTable[ph->first])
				{
					actionTable[row][column]=itemsetTable[ph->second];
					continue;
				}
				if(rightTable.count(ph->first))
				{
					actionTable[row][column]=itemsetTable[ph->second];
					continue;
				}
				if(leftTable.count(ph->first))//如果此操作符不在两个结合表中
				{
					continue;
				}
	
				//如果上述条件都不满足，则报错。
				cout<<"Reduction & Shift Confliction"<<endl;
				cout<<"row="<<row<<" column="<<column<<" old="<<actionTable[row][column]
					<<" new="<<itemsetTable[ph->second]
					<<" signal="<<ph->first<<endl;
				return false;
			}
			else
				actionTable[row][column]=itemsetTable[ph->second];
			//cout<<"actionTable row="<<row<<" column="<<column<<endl;
			//getch();
		}
	}
	return true;
	//到此完成了项目集到分析表的构造
}
void generateTableCode()
{
	fout<<"int actionTable["<<actionTable.size()<<"]["<<actionTable.front().size()<<"]=\n\t\t{";
	for(int i=0;i<actionTable.size();i++)
	{
		fout<<"\t\t{";
		for(int j=0;j<actionTable.front().size();j++)
		{
			if(actionTable[i][j]!=40000)
				fout<<actionTable[i][j];
			else
				fout<<"E";
			if(j==actionTable.front().size()-1) continue;
			fout<<",";
		}
		fout<<"}";
		if(i==actionTable.size()-1) continue;
		fout<<",\n";
	}
	fout<<"};\n";
	fout<<"int searchTable(int cstate,char symbol)\n";
	fout<<"{\n";
	fout<<"\treturn actionTable[cstate][symbol];\n";
	fout<<"}\n";
	fout<<"\n";
	//此部分完成查表程序的输出
	//下面完成读Token程序的输出
	fout<<"int readToken()\n";
	fout<<"{\n";
	fout<<"\tif(fin.eof())\n";
	fout<<"\t\treturn SOURCE_END;\n";
	fout<<"\tchar buf[256];//不支持超过256个字符的符号\n";
	fout<<"\tint pos=0;\n";
	fout<<"\tbool isEnd=false;\n";
	fout<<"\tbool isToken=false;//与isOperator互斥\n";
	fout<<"\tbool isOperator=false;\n";
	fout<<"\tstring ops(\"!@#$%^&*()+-=|\[]{};':\\\",.<>/?\");\n";
	fout<<"\tstring wss(\"\\t\\n\\r \");\n";
	fout<<"\tstring letter(\"_0123456789aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ\");\n";

	fout<<"\tchar c=fin.get();\n";
	fout<<"\twhile(c==' '||c=='\\t'||c=='\\n'||c=='\\r') c=fin.get();//滤掉空白符\n";
	fout<<"\twhile(!isEnd)\n";
	fout<<"\t{\n";
	fout<<"\t\tif(c==-1)\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tbuf[pos]='\\0';\n";
	fout<<"\t\t\tisEnd=true;\n";
	fout<<"\t\t\tcontinue;\n";
	fout<<"\t\t}";
	fout<<"\t\tif(ops.find(c)>=0&&ops.find(c)<ops.size())//表示当c不是正常字符时。\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\t//进入非正常字符的处理过程\n";
	fout<<"\t\t\tif(isToken)\n";
	fout<<"\t\t\t{\n";
	fout<<"\t\t\t\tfin.unget();\n";
	fout<<"\t\t\t\tbuf[pos]='\\0';\n";
	fout<<"\t\t\t\tisEnd=true;\n";
	fout<<"\t\t\t}\n";
	fout<<"\t\t\telse\n";
	fout<<"\t\t\t{\n";
	fout<<"\t\t\t\tisOperator=true;\n";
	fout<<"\t\t\t\tbuf[pos]=c;\n";
	fout<<"\t\t\t\tpos++;\n";
	fout<<"\t\t\t\tc=fin.get();\n";
	fout<<"\t\t\t}\n";
	fout<<"\t\t\tcontinue;\n";
	fout<<"\t\t}\n";
	fout<<"\t\tif(letter.find(c)>=0&&letter.find(c)<letter.size())\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tif(isOperator)\n";
	fout<<"\t\t\t{\n";
	fout<<"\t\t\t\tfin.unget();\n";
	fout<<"\t\t\t\tbuf[pos]='\\0';\n";
	fout<<"\t\t\t\tisEnd=true;\n";
	fout<<"\t\t\t}\n";
	fout<<"\t\t\telse\n";
	fout<<"\t\t\t{\n";
	fout<<"\t\t\t\tisToken=true;\n";
	fout<<"\t\t\t\tbuf[pos]=c;\n";
	fout<<"\t\t\t\tpos++;\n";
	fout<<"\t\t\t\tc=fin.get();\n";
	fout<<"\t\t\t}\n";
	fout<<"\t\t\tcontinue;\n";
	fout<<"\t\t}\n";
	fout<<"\t\tif(wss.find(c)>=0&&wss.find(c)<wss.size())\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tbuf[pos]='\\0';\n";
	fout<<"\t\t\tisEnd=true;\n";
	fout<<"\t\t}\n";
	fout<<"\t\t\n";
	fout<<"\t\t\n";

	fout<<"\t\telse return -1;\n";
	fout<<"\t}\n";
	fout<<"\treturn analysis(buf,strlen(buf));\n";
	fout<<"}\n";
}

void generateSemanticActionCode()
{
	fout<<"void runaction(int num)\n";
	fout<<"{\n";
	fout<<"\tswitch(num)\n";
	fout<<"\t{\n";
	for(int i=0;i<produceActionTable.size();i++)
	{
		if(produceActionTable[i].size()!=0)
		{
			fout<<"\tcase "<<i+1<<":\n";
			fout<<"\t\t{\n";
			fout<<"\t\t\t"<<produceActionTable[i]<<"\n";
			fout<<"\t\t\tbreak;\n";
			fout<<"\t\t}\n";
		}
	}
	fout<<"\t}\n";
	fout<<"}\n";
}

void generateParseCode()
{
	fout<<"int parse()\n";
	fout<<"{\n";
	fout<<"\tint inputsymbol=0;\n";
	fout<<"\tint cstate=0;\n";
	fout<<"\tstack<Sym> symStack;//符号栈\n";
	fout<<"\tstack<SV> valStack;//语义值栈\n";
	fout<<"\tSym st;//用作分析时的临时栈顶元素存储变量\n";
	fout<<"\tst.symbol=0;\n";
	fout<<"\tst.state=0;\n";
	fout<<"\tSV val;\n";
	fout<<"\tsymStack.push(st);//语义值栈必须要和符号栈同步\n";
	fout<<"\tvalStack.push(val);\n";
	fout<<"\tinputsymbol=readToken();\n";
	fout<<"\twhile(1)\n";
	fout<<"\t{\n";
	fout<<"\t\tst=symStack.top();\n";
	fout<<"\t\tint col=signalTable[inputsymbol];\n";
	fout<<"\t\tint result=searchTable(st.state,col);\n";
	fout<<"\t\tif(result==E)//出错\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tcout<<\"Compile Error!\"<<endl;\n";
	fout<<"\t\t\treturn 0;\n";
	fout<<"\t\t}\n";
	fout<<"\t\tif(result==ACCEPT)\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tcout<<\"Compile sucessfully!\"<<endl;\n";
	fout<<"\t\t\treturn 1;\n";
	fout<<"\t\t}\n";
	fout<<"\t\tif(result<0)//负数表示为归约项目\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tresult*=-1;\n";
	fout<<"\t\t\tint n=producerN[result];//取得该号产生式右部符号数量，以作弹栈用\n";
	fout<<"\t\t\tfor(int i=0;i<n;i++)\n";
	fout<<"\t\t\t{\n";
	fout<<"\t\t\t\tsymStack.pop();\n";
	fout<<"\t\t\t\tu[n-i]=valStack.top();\n";
	fout<<"\t\t\t\tvalStack.pop();\n";
	fout<<"\t\t\t}\n";
	fout<<"\t\t\trunaction(result);//执行语义动作\n";
	fout<<"\t\t\t//再将产生式左部的符号压栈，语义值一同压栈\n";
	fout<<"\t\t\tst.symbol=pLeftSection[result];\n";
	fout<<"\t\t\tst.state=searchTable(symStack.top().state,signalTable[st.symbol]);\n";
	fout<<"\t\t\tsymStack.push(st);\n";
	fout<<"\t\t\t//将产生式左部符号的语义值入栈。这个值在runaction()中已经修改\n";
	fout<<"\t\t\tvalStack.push(u[0]);\n";
	fout<<"\t\t}\n";
	fout<<"\t\telse\n";
	fout<<"\t\t{\n";
	fout<<"\t\t\tst.symbol=inputsymbol;\n";
	fout<<"\t\t\tst.state=result;\n";
	fout<<"\t\t\tsymStack.push(st);\n";
	fout<<"\t\t\tSV tu;\n";
	fout<<"\t\t\tif(yyval.ival==0)\n";
	fout<<"\t\t\t\tgetvalue(inputsymbol,tu);\n";
	fout<<"\t\t\telse tu=yyval;\n";
	fout<<"\t\t\tvalStack.push(tu);\n";
	fout<<"\t\t\tinputsymbol=readToken();\n";
	fout<<"\t\t}\n";
	fout<<"\t}\n";
	fout<<"}\n";
}

void generateMainCode()
{
	fout<<"void main()\n";
	fout<<"{\n";
	/*
	for(hash_map<string,int>::iterator pti=tsymTable.begin();pti!=tsymTable.end();pti++)
	{
		if(pti->first!="#")
		fout<<"\tconst int "<<pti->first<<"="<<pti->second<<";\n";//定义各变量，变量名用
	}*/
	fout<<"\tpair<int,int> tp;\n";
	for(hash_map<string,int>::iterator pti=tsymTable.begin();pti!=tsymTable.end();pti++)
	{
		if(pti->first!="#")
		{
			fout<<"\ttp.first="<<pti->second<<";\n";
			fout<<"\ttp.second="<<terminSet[pti->second]<<";\n";
			fout<<"\tsignalTable.insert(tp);\n";
		}
		else
		{
			fout<<"\ttp.first=40001;\n";
			fout<<"\ttp.second="<<terminSet[pti->second]<<";\n";
			fout<<"\tsignalTable.insert(tp);\n";
		}
	}
	for(hash_map<int,int>::iterator pti=nonterminSet.begin();pti!=nonterminSet.end();pti++)
	{
		fout<<"\ttp.first="<<pti->first<<";\n";
		fout<<"\ttp.second="<<pti->second<<";\n";
		fout<<"\tsignalTable.insert(tp);\n";
	}
	//以上完成signalTable的初始化段代码.
	fout<<"\tstring filename;\n";
	fout<<"\tcout<<\"Please input the file name:\"<<endl;\n";
	fout<<"\tcin>>filename;\n";
	fout<<"\tfin.open(filename.c_str());\n";
	fout<<"\tif(fin.fail())\n";
	fout<<"\t{\n";
	fout<<"\t\tcout<<\"Cannot open the file \"<<filename<<endl;\n";
	fout<<"\t\treturn;\n";
	fout<<"\t}\n";
	fout<<"\tparse();\n";
	fout<<"}\n";
}
void generateSVCode()
{
	fout<<"void getvalue(int symbol,SV & val)\n";
	fout<<"{\n";
	fout<<"\tswitch(symbol)\n";
	fout<<"\t{\n";
	for(hash_map<int,int>::iterator pi=terminSet.begin();pi!=terminSet.end();pi++)
	{
		if(symclaTable.count(pi->first))
		fout<<"\tcase "<<pi->first<<":val."<<symclaTable[pi->first]<<"="<<pi->first
			<<";break;\n";
	}
	fout<<"\t}\n";
	fout<<"}\n";


}

void generateConstCode()
{
	fout<<"using namespace std;\n";
	fout<<"extern int analysis(char *yytext,int n);\n";
	fout<<"struct Sym\n";
	fout<<"{\n";
	fout<<"\tint symbol;\n";
	fout<<"\tint state;\n";
	fout<<"};\n";
	fout<<"hash_map<int,int> signalTable;\n";
	fout<<"int producerN["<<producerSet.size()<<"]={";
	for(int i=0;i<producerSet.size();i++)
	{
		fout<<producerSet[i].right.size();
		if(i!=producerSet.size()-1)
			fout<<",";
	}
	fout<<"};\n";
	fout<<"int pLeftSection["<<producerSet.size()<<"]={";
	for(i=0;i<producerSet.size();i++)
	{
		fout<<producerSet[i].left;
		if(i!=producerSet.size()-1)
			fout<<",";
	}
	fout<<"};\n";
	fout<<"ifstream fin;\n";
}
void generateHead()
{
	fout<<"#include<iostream>\n";
	fout<<"#include<hash_map>\n";
	fout<<"#include<stack>\n";
	fout<<"#include<fstream>\n";
	fout<<"#include<string>\n";
	fout<<"#define E 40000\n";
	fout<<"#define ACCEPT 0\n";
	fout<<"#define SOURCE_END 40001\n";	
}

int readInputFile()
{
	generateHead();
	char c=fin.get();
	if(c!='%') return TERMINATE;//要求输入文件第一个字符必须为%
	int state=specSymParse();
	while(!fin.eof()&&c!=-1)
	{
		switch(state)
		{
		case SEGMENT_DELIM:
			{
				NO++;
				if(NO==2)
				{
					state=SEGMENT_REG;//进入规则段
					continue;//跳过后面的查找%的语句，直接回到switch
				}
				if(NO==3)
				{
					state=SELF_DEF;
					continue;
				}
				break;
			}
		case DEF_SEG_BEGIN://output 1
			{
				//进入%{处理，完全输出到文件
				while(1)
				{
					c=fin.get();
					if(c=='%')
					{
						state=specSymParse();
						if(state==DEF_SEG_END) break;//如果到达底部%}，则退出循环。
						else if(state!=NOMATCH)
						{
							return TERMINATE;//如果既不等于DEF_SEG_END也不等于NOMATCH，则报错。
						}
					}
					fout.put(c);///////********output to file
				}
				break;
			}
		case TOKEN:case TYPE:
			{
				string line;
				getline(fin,line);

				int lpos=0;
				int pos=0;
				while(line[pos]!='<'&&pos<line.size()) pos++;
				if(pos==line.size())
				{
					cout<<"%type formation error!"<<endl;
					return 0;
				}
				
				pos++;//往后移一个字符
				lpos=pos;
				while(line[pos]!='>'&&pos<line.size()) pos++;
				if(pos==line.size())
				{
					cout<<"%type or token formation error!"<<endl;
					return 0;
				}
				string valstr=line.substr(lpos,pos-lpos);//取得值类型

				char ntbuf[20]={0};
				int i=0;
				while(1)
				{
					pos++;
					if(pos>line.size()) break;
					if(line[pos]==' '||pos==line.size())
					{
						if(ntbuf[0]=='\0') continue;
						ntbuf[i]='\0';
						string ntstr(ntbuf);

						pair<string,int> tp;
						tp.first=ntstr;
						if(state==TYPE)
						{
							tp.second=NONTERMINS+ntsymTable.size();
						}
						else
						{
							tp.second=TERMINS+tsymTable.size();
						}
						pair<int,int> tp2;
						tp2.first=tp.second;
						tp2.second=terminSet.size()+nonterminSet.size();

						if(state==TYPE)
						{
							ntsymTable.insert(tp);
							nonterminSet.insert(tp2);
						}
						else
						{
							tsymTable.insert(tp);
							terminSet.insert(tp2);
						}

						pair<int,string> stp;
						stp.first=tp.second;
						stp.second=valstr;
						symclaTable.insert(stp);
						
						i=0;
						ntbuf[0]='\0';
					}
					else
					{
						ntbuf[i]=line[pos];	
						i++;
					}
				}
				break;
				//此部分处理完之后，完成了表symclaTable和部分ntsymTable
			}
		case UNION://output 2
			{
				c=fin.get();
				while(c!='{'&&c!=-1) c=fin.get();
				if(c==-1) return TERMINATE;	
				hout<<"#ifndef YYVAL_H\n";//输出到头文件yyval_h.h
				hout<<"#define YYVAL_H\n";
				fout<<"union SV\n";
				hout<<"union SV\n";
				fout<<"{\n";
				hout<<"{\n";
				while((c=fin.get())!='}') {fout.put(c);hout.put(c);}
				fout<<"};\n";//此段没有做任何保存，即是说如果出现错误也不能校错。
				hout<<"};\n";
				fout<<"SV u[10];\n";//在此声明数目为10的联合数组，以便后面语义动作时调用。
				fout<<"SV yyval;\n";//在此声明全局变量yyval，用于返回lex符号的语义值
				hout<<"#endif";
				break;
			}
		case LEFT:case RIGHT:
			{
				string line;
				getline(fin,line);
				int lpos=0;
				int pos=0;
				static int priority=0;
				priority++;
				while(pos!=line.find_last_of('\''))
				{
					lpos=line.find_first_of('\'',lpos);
					pos=line.find_first_of('\'',lpos+1);
					string t=line.substr(lpos+1,pos-lpos-1);
					lpos=pos+1;	
					//存储操作符。
					pair<string,int> tp;
					tp.first=t;
					tp.second=TERMINS+tsymTable.size();
					tsymTable.insert(tp);//把操作符也放到终结符表中。
				    
					pair<int,int> tpp;
					tpp.first=tp.second;
					tpp.second=terminSet.size()+nonterminSet.size();
					terminSet.insert(tpp);
					if(state==LEFT)//加入左右结合表
					{
						leftTable.insert(tp.second);
					}
					else
					{
						rightTable.insert(tp.second);
					}

					pair<int,int> ptp;
					ptp.first=tp.second;
					ptp.second=priority;
					precedenceTable.insert(ptp);//把操作符放入优先级表中。
				}
				break;
			}
		case SEGMENT_REG:
			{
				//此处为规则段的扫描和处理输出。
				//第一步读产生式的集合，完成各种表的数据。			
				//修正producerPreTable		
				producerPreTable.push_back(0);//保证索引值访问不会出错。
				if(!readReg())
				{
					cout<<"read reg segment error!\n";
					return 0;
				}
				//修正ProducerSet;
				producerSet[0].left=NONTERMINS-1;//表示是拓展文法开始符号
				producerSet[0].right.push_back(producerSet[1].left);
				//修正tsymTable
				pair<string,int> tp1;
				string s("#");
				tp1.first=s;
				tp1.second=NONTERMINS-1;
				tsymTable.insert(tp1);
				//修正terminSet
				pair<int,int> tp2;
				tp2.first=tp1.second;
				tp2.second=terminSet.size()+nonterminSet.size();
				terminSet.insert(tp2);
				//第二步完成动作表的生成。
				genhpSet();//首先完成hpSet的生成
				if(!produce())
				{
					cout<<"produce action table error!"<<endl;
					return 0;
				}

				//输出文件定义的一些常量
				generateConstCode();
				//第三步输出查表函数及读Token的函数
				generateTableCode();
				//第四步输出执行语义动作的函数
				generateSemanticActionCode();
				//第五步输出分析函数,首先产生void getvalue()函数，给终结符赋默认初值
				generateSVCode();
				generateParseCode();
				//第六步输出主函数，应该留到最后输出。
				//generateMainCode();
				break;
			}
		case SELF_DEF://输出用户自定义子例程
			{
				c=fin.get();
				while(c!=-1)
				{
					fout.put(c);////*******output to file
					c=fin.get();
					
				}
				fout.put('\n');
				continue;//跳回while，判断结束。
			}
		}
		c=fin.get();
		while(c!='%'&&c!=-1) c=fin.get();
		if(c=='%')
			state=specSymParse();
	}
}
int specSymParse()
{
	char nextc=fin.get();
	switch(nextc)
	{
	case '%'://进入段分隔符
		{
			return SEGMENT_DELIM;
		}
	case '{':
		{
			return DEF_SEG_BEGIN;
		}
	case '}':
		{
			return DEF_SEG_END;
		}
	case 't':case 'u':case 'l':case 'r'://%token	//%union//%left
		{
			char buf[10];
			int pos=1;
			buf[0]=nextc;
			nextc=fin.get();
			while(nextc!=' '&&nextc!='\n'&&nextc!='\t')
			{
				buf[pos]=nextc;
				nextc=fin.get();
				pos++;
				if(pos==5) break;
			}
			buf[pos]='\0';
			string bbuf(buf);
			string t1("token");
			string t2("type");
			string t3("union");
			string t4("left");
			string t5("right");
			if(bbuf==t1) return TOKEN;
			if(bbuf==t2) return TYPE;
			if(bbuf==t3) return UNION;
			if(bbuf==t4) return LEFT;
			if(bbuf==t5) return RIGHT;
			else return TERMINATE;//错误，程序中止
		}

	default:
		fin.unget();
		return NOMATCH;
	}
}

bool readReg()
{
	//规则段的扫描程序
	char c=fin.get();
	while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();
	int state=-1;
	char buf[256];
	int pos=0;
	while(1)
	{
		if(c==-1) break;
		if(c=='%')
		{
			state=specSymParse();
			if(state==SEGMENT_DELIM)
			{
				fin.unget();
				fin.unget();//连退两个字符，因为在readInputFile()里面有对%%进行处理的程序
				break;
			}
			if(state==TERMINATE) 
			{
				cout<<"Regulation Segment ERROR!"<<endl;
				return false;
			}	
			if(state==NOMATCH) fin.unget();//将%也退回，留作下面程序处理
			else
			{
				cout<<"Regulation Segment ERROR2"<<endl;
				return false;//出错，直接返回.
			}
		}


		while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();//抛掉空格
		fin.unget();//退回，交给函数readAproducer()处理
		if(!readAproducer())
		{
			cout<<"producer error!see above!"<<endl;
			return false;
		}
		c=fin.get();
		while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();
	}

}
bool readAproducer()//未抛掉前后的空格
{
	char c=fin.get();
	char buf[256];
	int pos=0;
	while(c!=' '&&c!='\n'&&c!='\t'&&c!='\r'&&c!=-1)
	{
		buf[pos]=c;
		pos++;
		c=fin.get();
	}
	fin.unget();
	buf[pos]='\0';
	string temp(buf);
	buf[0]='\0';//清空buf。
	pos=0;
	if(tsymTable.count(temp))
	{
		cout<<"NonTerminatedSym define error!"<<endl;
		return false;
	}
	if(!ntsymTable.count(temp))
	{
		pair<string,int> tp;
		tp.first=temp;
		tp.second=NONTERMINS+ntsymTable.size();
		ntsymTable.insert(tp);
	
		pair<int,int> tpp;
		tpp.first=tp.second;
		tpp.second=terminSet.size()+nonterminSet.size();
		nonterminSet.insert(tpp);//往非终结符表内加入内容。存储的是非终结符对应到action表的表头
	}
	c=fin.get();
	while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();
	if(c!=':')
	{
		cout<<"Producer formation Error!"<<endl;
		return false;
	}
	while(1)
	{
		Producer ap;
		ap.left=ntsymTable[temp];//新建一个产生式，先保存左部的值
		int priority=0;//存储产生式的优先级，此时采取的方法是，产生式的优先级同右部符号最右的操作
	               //符优先级相同
		while(1)
		{
		//首先抛掉空格
			c=fin.get();
			while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();
			if(c==';'||c=='{'||c=='|') break;//产生式结束标记或者是动作开始标记
			//读产生式的右部
			if(c=='\'')//如果遇到'，则进入操作符的读入程序
			{
				c=fin.get();
				while(c!='\'')
				{
					buf[pos]=c;
					pos++;
					c=fin.get();
				}
				buf[pos]='\0';
				string temp3(buf);
				pos=0;
				buf[0]='\0';//clear buf
				if(!tsymTable.count(temp3))
				{
					pair<string,int> tp;
					tp.first=temp3;
					tp.second=TERMINS+tsymTable.size();

					tsymTable.insert(tp);//把操作符也放到终结符表中。
				
					pair<int,int> tpp;
					tpp.first=tp.second;
					tpp.second=terminSet.size()+nonterminSet.size();
					terminSet.insert(tpp);
					//ap.right.push_back(tp.second);

					pair<int,int> ptp;
					ptp.first=tp.second;
					ptp.second=0;
					precedenceTable.insert(ptp);
				}
				ap.right.push_back(tsymTable[temp3]);
				priority=precedenceTable[tsymTable[temp3]];
				continue;
			}
			if(c=='\\')//用\e表示空
			{
				c=fin.get();
				if(c=='e')
				{
					break;
				}
				else
				{
					cout<<"epslong error!"<<endl;
					return false;
				}
			}
			else
			{
				while(c!=' '&&c!=-1&&c!='\n'&&c!='\t'&&c!='\r')
				{
					buf[pos]=c;
					pos++;
					c=fin.get();
				}
				buf[pos]='\0';
				string temp1(buf);
				pos=0;
				buf[0]='\0';//clear buf
				if(tsymTable.count(temp1))
				{
					ap.right.push_back(tsymTable[temp1]);
				}
				else
				{
					if(!ntsymTable.count(temp1))
					{
						pair<string,int> tp;
						tp.first=temp1;
						tp.second=NONTERMINS+ntsymTable.size();
						ntsymTable.insert(tp);
	
						pair<int,int> tpp;
						tpp.first=tp.second;
						tpp.second=terminSet.size()+nonterminSet.size();
						nonterminSet.insert(tpp);

					}
					ap.right.push_back(ntsymTable[temp1]);
				}
			}
		}
		char action[1000];//存储语义动作
		action[0]='\0';
		if(c=='{')
		{
			char change[4]={'u','[','0',']'};
			int pi=0;
			c=fin.get();
			while(c!='}')
			{
				if(c=='$')
				{
					c=fin.get();
					if(c=='$')//表示是产生式左部的语义值符号
					{
						//插入串"u[0]"
						for(int i=0;i<4;i++)
							action[pi+i]=change[i];
						pi+=4;
						action[pi]='.';//加上语义值的具体类型
						pi++;
						string s=symclaTable[ap.left];
						for(i=0;i<s.size();i++)
							action[pi+i]=s[i];
						pi+=i;
					}
					else 
					if(c>='1'&&c<='9')
					{
						int num=0;
						action[pi++]='u';
						action[pi++]='[';
						while(c>='0'&&c<='9')//支持更多的右部非终结符
						{
							action[pi++]=c;
							num=num*10+c-'0';
							c=fin.get();
						}
						fin.unget();
						action[pi++]=']';
						/*
						for(int i=0;i<4;i++)
						{
							action[pi+i]=change[i];
						}*/
						//action[pi+2]=c;//只需修改此处的值即可。
						//pi+=4;
						action[pi++]='.';//加上语义值的具体类型
						string s=symclaTable[ap.right[num-1]];
						for(int i=0;i<s.size();i++)
							action[pi+i]=s[i];
						pi+=i;
					}
					else
					{
						cout<<"Action definition error!"<<endl;
						return false;
					}
				}
				else
				{
					action[pi]=c;
					pi++;	
				}
				c=fin.get();
			}
			action[pi]='\0';
			c=fin.get();
			while(c==' '||c=='\n'||c=='\r'||c=='\t') c=fin.get();
		}
		if(c==';'||c=='|')
		{
			//保存语义动作。
			string oneaction(action);
			produceActionTable.push_back(oneaction);
			producerSet.push_back(ap);
			producerPreTable.push_back(priority);
			if(c==';')
				break;
		}
		else
		{
			cout<<"Producer formation error,there should be a ';' after each producer!"<<endl;
			return false;
		}
	}
	return true;
}


