#include "windows.h"
#include <iostream>
#include <string>
using namespace std;
int main()
{
	//创建句柄
	HANDLE hFileRead;
	HANDLE hFileWrite;
	//数据长度
	DWORD dwDataLen = 100;  
	DWORD written = 0;   
	//数据缓存
	char buffer[101];   

	//提示并输入源文件和目标文件名
	string inputFile;
	string outputFile;
	cout<<"Please input the file name of source file:" <<endl;
	cin>>inputFile;
	cout<<"Please input the file name of destination file:" <<endl;
	cin>>outputFile;

	//打开文件   
	if ((hFileRead=CreateFile(inputFile.c_str(),   
		GENERIC_READ,   
		FILE_SHARE_READ,   
		NULL,   
		OPEN_EXISTING,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL)) == INVALID_HANDLE_VALUE )
	{
		cout<<"The temp to open file makes error! "<<endl;
		return 0;
	}

	if (( hFileWrite=CreateFile(outputFile.c_str(),   
		GENERIC_WRITE,   
		FILE_SHARE_WRITE,   
		NULL,   
		OPEN_ALWAYS,   
		FILE_ATTRIBUTE_NORMAL,   
		NULL) )== INVALID_HANDLE_VALUE)
	{
		cout<<"The temp to open file makes error! "<<endl;
		exit(1);
	}

	while( dwDataLen != 0 )
	{
		//读取数据   
		ReadFile(hFileRead,buffer,99,&dwDataLen,NULL);
		//标记结束位置   
		buffer[dwDataLen]='\0';
		SetFilePointer(hFileWrite, 0, NULL, FILE_END);
		WriteFile(hFileWrite,buffer,dwDataLen,&written,NULL);//写入文件   
	}
	
	//关闭句柄   
	CloseHandle(hFileRead);
	CloseHandle(hFileWrite);

	//输出成功提示
	cout<<"The operation of copying has terminated normally!"<<endl;
	return 0;
}