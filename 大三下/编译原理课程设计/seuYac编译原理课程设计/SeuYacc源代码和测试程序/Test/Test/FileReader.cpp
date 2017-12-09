#include "FileReader.h"
#include <string>
using std::string;

ruleSet rs;
FileReader::FileReader()
{
	specSym["%token"] = TOKEN;
	specSym["%type"] = TYPE;
	specSym["%union"] = UNION;
	specSym["%left"] = LEFT;
	specSym["%right"] = RIGHT;
	specSym["%nonassoc"] = NONASSOC;
	lineNo = 0;
}


FileReader::~FileReader()
{
}

int FileReader::ParseYFile(char * fileName)
{
	fin.open(fileName);
	if (fin.fail())
	{
		std::cout << "Cannot open output file!" << std::endl;
		return 0;
	}
	getDefPart();
	getRegPart();
	getCodePart();
	return 0;
}

void FileReader::getDefPart()
{
	string temp, ret;
	yaccState state = NORMAL;
	int priority = 1;
	bool specFlag = false, endFlag = false;
	while(specFlag||getline(fin, temp, '\n'))
	{
		lineNo++;
		specFlag = false;
		if (temp == "")
			continue;
		if (endFlag)
			break;
		int ind = 0;
		while (temp[ind] == '\t' || temp[ind] == ' ')
			ind++;
		if ((size_t)ind >= temp.length())
			continue;
		if (state==NORMAL&&temp.length() - ind <= 1)
		{
			std::cout << "error in yacc file line" << lineNo << std::endl;
			break;
		}
		switch (state)
		{
		case NORMAL:
			if (temp[ind] == '%'&&temp[ind + 1] == '{')
			{
				state = EXT;
				specFlag = true;
				temp = temp.substr(ind + 2, temp.length() - ind - 2);
				break;
			}
			if (temp[ind] == '%'&&temp[ind + 1] == '%')
			{
				endFlag = true;
				break;
			}
			char copystr[255],*p;
			memset(copystr, 0, sizeof(copystr));

			temp.copy(copystr, temp.length(), 0);
			ret=strtok(copystr, "\t ");
			//std::cout << ret<<std::endl;
			switch (getState(ret))
			{
				case TOKEN:
					p = strtok(NULL, "\t \n");
					while (p)
					{
						ret = p;
						rs.Token.insert(ret);
						p = strtok(NULL, "\t \n");
					}
					break;
				case TYPE:break;
				case UNION:
					unionCode.push_back(temp);
					while (getline(fin, temp, '\n'))
					{
						lineNo++;
						bool unionF = false;
						unionCode.push_back(temp);
						for (int i = 0; i < temp.length(); i++)
						{
							if (temp[i] == '}')
							{
								unionF = true;
								break;
							}		
						}
						if (unionF)
							break;
					}
					break;
				case LEFT:
					p = strtok(NULL, "\t \n");
					while (p)
					{
						ret = p;
						rs.priority[ret] = priority;
						rs.Left.insert(ret);
						p = strtok(NULL, "\t \n");
					}
					priority++;
					break;
				case RIGHT:
					p = strtok(NULL, "\t ");
					while (p)
					{
						ret = p;
						rs.priority[ret] = priority;
						rs.Right.insert(ret);
						p = strtok(NULL, "\t \n");
					}
					priority++;
					break;
				case NONASSOC:
					p = strtok(NULL, "\t \n");
					while (p)
					{
						ret = p;
						rs.priority[ret] = priority;
						rs.Nonassoc.insert(ret);
						p = strtok(NULL, "\t \n");
					}
					priority++;
					break;
				default:
					std::cout << "Undefined identity " << ret << " in line " << lineNo << std::endl;				
				break;
			}
			break;
		case EXT:
			if (temp[ind] == '%'&&temp[ind + 1] == '}')
			{
				state = NORMAL;
				specFlag = true;
				temp = temp.substr(ind + 2, temp.length() - ind - 2);
				break;
			}
			preCode.push_back(temp);
			break;
		}
		if (endFlag)
			break;

	}
}
bool isLegalChar(char ch)
{
	return ch>32 && ch<127;
}

void FileReader::getRegPart()
{
	vector<Symbol> symbolRule;
	char inBuffer[255];
	int inBufferIndex = 0;
	char inch;
	int sID = 0;
	string tempStr;
	string headStr;
	int headFlag = 1;//头部分
	int ruleFlag = 1;//规则部分
	int semFlag = 0;//语义Flag
	int comFlag = 0;//注释Flag
	int endFlag = 0;
	while (fin.get(inch),~inch)
	{
		if (endFlag)
			break;
		if (isLegalChar(inch))
		{
			switch (inch)
			{
			case ':':
				if (semFlag || comFlag)
					break;
				if (inBufferIndex)
				{
					inBuffer[inBufferIndex++] = '\0';
					headStr = inBuffer;
				
				}
				else
				{
					headStr = tempStr;					
				}
				headFlag = 0;
				break;
			case '|':
				if (semFlag || comFlag)
					break;
				if (inBufferIndex)
				{
					inBuffer[inBufferIndex++] = '\0';
					tempStr = inBuffer;
					int tflag = 0;
					if (rs.Token.count(tempStr) == 0)
						tflag = 0;
					else
						tflag = 1;
					Symbol tempS(tempStr, tflag);
					symbolRule.push_back(tempS);
				}
				rs.addRule(rule(Symbol(headStr, 0), symbolRule));
				symbolRule.clear();
				break;
			case ';':
				if (semFlag || comFlag)
					break;
				rs.addRule(rule(Symbol(headStr, 0), symbolRule));
				symbolRule.clear();

				headFlag = 1;
				break;
			case '{':
				if (comFlag)
					break;
				semFlag = 1;
				if (inBufferIndex)
				{
					inBuffer[inBufferIndex++] = '\0';
					tempStr = inBuffer;
					if (headFlag)
						;
					else
					{
						int tflag = 0;
						if (rs.Token.count(tempStr) == 0)
							tflag = 0;
						else
							tflag = 1;
						Symbol tempS(tempStr, tflag);
						symbolRule.push_back(tempS);
					}
					inBufferIndex = 0;
				}
				inBufferIndex = 0;
				break;
			case '}':
				if (comFlag)
					break;
				semFlag = 0;
				break;
			case '%':
				if (semFlag || comFlag)
					break;
				fin.get(inch);
				if (inch == '%')
					endFlag = 1;	
				break;

			default:
				if (semFlag || comFlag)
					break;
				inBuffer[inBufferIndex++] = inch;
			}

		}
		else
		{
			if (semFlag || comFlag)
				continue;
			if (inBufferIndex)
			{
				inBuffer[inBufferIndex++] = '\0';
				tempStr = inBuffer;
				if (headFlag)
					;
				else
				{
					int tflag = 0;
					if (rs.Token.count(tempStr) == 0)
						tflag = 0;
					else
						tflag = 1;
					Symbol tempS(tempStr, tflag);
					symbolRule.push_back(tempS);
				}
				inBufferIndex = 0;
			}
			inBufferIndex = 0;
		}
		if (endFlag)
			break;

	}
}

void FileReader::getCodePart()
{
	string temp;
	while (getline(fin, temp, '\n'))
	{
		userCode.push_back(temp);
		//cout << temp;
	}
}

yaccState FileReader::getState(string str)
{
	return specSym[str];
}
