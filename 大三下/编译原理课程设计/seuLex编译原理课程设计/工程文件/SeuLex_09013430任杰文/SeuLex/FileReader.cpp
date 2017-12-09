#include "FileReader.h"

FileReader::FileReader(string fileName)
{
	fcin.open(fileName, ios::in);

	// 定义regularExpressionNum
	regularExpressionNum = 0;
	// 定义definePartNum
	definePartNum = 0;
	// 定义lastPartNum
	lastPartNum = 0;
}
FileReader::~FileReader()
{
	fcin.close();
}
vector<string> FileReader::getRegularExpression() 
{
	// 计算regularExpressionNum
	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置
	string s;
	for(int i=0; i<2; i++)
	{
		while(fcin)
		{
			getline(fcin, s);
			if(s.size()==0 || s[0]==' ' || s[0]=='\t')
			{
				continue;
			}
			if(s == "%%")
			{
				regularExpressionNum++;
				break;
			}
			if(regularExpressionNum != 0)
			{
				regularExpressionNum++;
			}
		}
	}
	regularExpressionNum -= 2; //减去首尾两次

	//将指针移到文件首位置
	fcin.clear();
	fcin.seekg(0, ios::beg); 
	while(fcin)
	{
		getline(fcin, s);
		//开始位置
		if(s=="%%")
		{
			break;
		}
	}

	vector<string> ret(regularExpressionNum);
	for( int i = 0;  ; i++)
	{
		//结束条件
		getline( fcin, s);
		if(s == "%%")
			break;
		int len = s.size();
		if(len==0 || s[0]==' ' || s[0]=='\t')
		{
			i--; //让ret不会因为Cminus.l中的空行出现空值
			continue;
		}
		int temp = 0;
		for( int j = 0; j < len;  j++ )
		{
			//匹配“”
			if (s[j]=='"')
				temp++;
			if((s[j]==' ' || s[j]=='\t') && temp %2 ==0) 
			{
				ret[i] = s.substr(0, j);				
				break;
			}
		}
	}
	return ret;
}
vector<string> FileReader::getCode() 
{
	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置

	string s;
	while(fcin)
	{
		getline(fcin, s);
		if(s=="%%")
		{
			break;
		}
	}
	vector<string> ret(regularExpressionNum);
	for(int i=0; ; i++)
	{
		getline(fcin, s);
		if(s == "%%")
		{
			break;
		}

		int len = s.size();
		if(len==0 || s[0]==' ' || s[0]=='\t')
		{
			i--; //让ret不会因为Cminus.l中的空行出现空值
			continue;
		}

		int temp = 0;
		for(int j=0; j<len; j++)
		{
			//配对 ”
			if (s[j]=='"')
				temp++;

			// temp是为了去除white space
			if( (s[j]==' ' || s[j]=='\t')
				&& ( j+1 ) < len
				&& s[j+1] != ' ' && s[j+1]!='\t' && temp %2==0)
			{
				ret[i] = s.substr(j+1, len-j-1);	
				break;
			}
		}
	}
	return ret;
}
vector<string> FileReader::getDefPart()  //cminus.l文件最前面的一段c代码
{
	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置

	string s;
	definePartNum = 0;
	while(fcin)
	{
		getline(fcin, s);
		if (s == "%{" || s.size()==0)
		{
			continue;
		}
		if (s == "%}")
		{
			break;
		}

		definePartNum++;
	}

	//string* ret = new string[definePartNum];
	vector<string> ret(definePartNum);
	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置
	int pos = 0;
	while(fcin)
	{
		getline(fcin, s);
		if (s == "%{" || s.size()==0)
		{
			continue;
		}
		if (s == "%}")
		{
			break;
		}
		ret[pos++] = s;
	}
	return ret;
}
vector<string> FileReader::getLastPart() 
{
	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置

	string s;
	bool isFirstTime = true;
	lastPartNum = 0;
	while(fcin)
	{
		getline(fcin, s);
		if (s.size()==0)
		{
			continue;
		}
		if (s == "%%" && isFirstTime)
		{
			isFirstTime = false;
			continue;
		}
		if (s == "%%" && !isFirstTime)
		{
			lastPartNum++;
		}
		if(lastPartNum)
		{
			lastPartNum++;
		}
	}
	lastPartNum -= 2; 

	vector<string> ret(lastPartNum);

	fcin.clear();
	fcin.seekg(0, ios::beg); //将指针移到文件首位置
	int pos = 0;
	isFirstTime = true;
	bool flag = true;
	while(getline(fcin, s))
	{
		//getline(fcin, s);
		if (s.size()==0)
		{
			continue;
		}
		if (s == "%%" && isFirstTime)
		{
			isFirstTime = false;
			continue;
		}
		if (s == "%%" && !isFirstTime)
		{
			flag = false;
			continue;
		}
		if(!flag)
		{
			ret[pos++] = s;
		}	
	}

	return ret;
}
map<string,string> FileReader:: getRegDefPart()
{
	fcin.clear();
	//定位文件头部
	fcin.seekg(0, ios::beg);

	//返回正规定义及对应正规表达式
	map<string,string> res;
	//当前行
	string str;

	//获取正规定义及其正规式
	char RegDef[20];
	char RegExp[20];
	while ( getline(fcin, str) && str != "%}" );
	while(getline(fcin, str) &&  str!="%%")
	{
		if(str != "")
		{
			sscanf( str.c_str(), "%s%s",&RegDef,&RegExp);
			res[string(RegDef)] = string(RegExp);
		}
	}
	return res ;
}