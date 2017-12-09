#ifndef TRANSTABLE_H
#define TRANSTABLE_H
#include "headers.h"
//NFA状态转化表
class Table 
{
public:
	Table(int pNumRows);
	~Table();
	//置位一组转化函数
	void set(int row, char column, hash_set<int> *items);
	//获取状态 row 接收符号column后转移到的状态集合
	hash_set<int>* get(int row, char column);
	//插入一个转化函数
	void add(int row, int column, int item);
	//浅拷贝一行表项
	void copyLineByRef(Table *from, int fromLine, int toLine);
	//返回表行数
	int getNumRows();

private:
	//状态转化表行数（状态数）
	int numRows;
	//存储hash_set<int>指针的二维数组
	vector<vector<hash_set<int>*> > tableItems; 
};
#endif