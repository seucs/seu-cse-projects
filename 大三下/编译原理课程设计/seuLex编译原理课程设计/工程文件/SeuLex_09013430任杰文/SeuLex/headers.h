#ifndef HEADERS_H
#define HEADERS_H
#include <string>
#include <fstream>
#include <iostream>
#include <cassert>
#include <map>
#include <set>
#include <stack>
#include <queue>
#include <vector>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <hash_set>
#include <hash_map>
#include <iomanip>
using namespace std;

//用ASCLL码不用的常量整数表示：闭包、正闭包、或空串、与、或、空串
const char STAR = 128;
const char ONE_OR_MORE = 129;
const char ONE_OR_NONE = 130;
const char AND = 131;
const char OR = 132;
const char LEFT_LITTLE_BRACKET = 133;
const char RIGHT_LITTLE_BRACKET = 134;
//定义空串ascll码为0
const char EPSILON = 0;
//定义NFA状态转化表列数，即NFA可接收符号数（为ASCLL码 0 - 127 ）
const int NUM_OF_COLUMNS = 128;
#endif