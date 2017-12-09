#include "TransTable.h"
Table::Table(int pNumRows) {
	//初始化状态转化表为空
	numRows = pNumRows;
	tableItems = vector<vector<hash_set<int>*>> (numRows,vector<hash_set<int>*>(NUM_OF_COLUMNS,NULL));
}
Table::~Table()
{
}
//置位一组转化函数
void Table::set(int row, char column, hash_set<int> *items) {
	tableItems[row][column] = items;
}
//获取状态 row 接收符号column后转移到的状态集合
hash_set<int>* Table::get(int row, char column) 
{
	return tableItems[row][column];
}
//插入一个转化函数
void Table::add(int row, int column, int item)
{
	if(tableItems[row][column] == NULL)
		tableItems[row][column] = new hash_set<int>();
	tableItems[row][column]->insert( item );
}
//浅拷贝一行表项
void Table::copyLineByRef(Table *from, int fromLine, int toLine) 
{
	tableItems[toLine] = from->tableItems[fromLine];
}

//返回表行数
int Table::getNumRows() 
{
	return numRows;
}