/*
 * copyFile.cpp
 *
 *  Created on: 2016年3月20日
 *      Author: rjw
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
using namespace std;
#define BUFFER_SIZE 1024
#define FILENAME_SIZE 10

int main() {
	int file_len = 0;
	int from_fd = -1;
	int to_fd = -1;
	int rest = 1;
	char buffer[BUFFER_SIZE];
	char inputFile[FILENAME_SIZE], outputFile[FILENAME_SIZE];
	char *ptr;

	//输入提示
	cout << "请输入源文件名：" << endl;
	cin >> inputFile;
	cout << "请输入目标文件名：" << endl;
	cin >> outputFile;

	//打开文件
	if ((from_fd = open(inputFile, O_RDONLY | O_CREAT)) == -1) {
		cout << "inputFile open error!" << endl;
		exit(1);
	}
	if ((to_fd = open(outputFile, O_WRONLY | O_CREAT)) == -1)
	{
		cout << "outputFile open error!" << endl;
		exit(1);
	}

	//检查文件长度
	file_len = lseek(from_fd, 0L, SEEK_END);
	lseek(from_fd, 0L, SEEK_SET);
	cout << "文件长度为：" << file_len<<endl;

	//文件复制
	while (rest) {
		rest = read(from_fd, buffer, BUFFER_SIZE);
		if (rest == -1) {
			cout<<"Read error!"<<endl;
			exit(1);
		}
		write(to_fd, buffer, rest);
		file_len -= rest;
		bzero(buffer, BUFFER_SIZE);
	}
	cout<<"there are "<<file_len<<" byte(s) data left without copy\n";

	//关闭文件
	close(from_fd);
	close(to_fd);
	return 0;
}

