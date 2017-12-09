#include <stdio.h> 
#include <windows.h>
#include <iostream>
#include <time.h>
using namespace std;

//定义缓冲区数据类型
typedef int buffer_item;
//定义缓冲区大小
#define BUFFER_SIZE 8 
//创建缓冲区
buffer_item myBuffer[BUFFER_SIZE];
//定义信号量
HANDLE empty;
HANDLE full;
HANDLE mutex;
//定义生产者、消费之缓冲区下标
int in = 0, out = 0;

//初始化函数
void init()
{	
	//初始化信号量
	empty = CreateSemaphore( NULL, 5, 5, NULL );
	full = CreateSemaphore( NULL, 0, 5, NULL );
	mutex = CreateSemaphore( NULL, 1, 1, NULL );
}

//插入一个数据到缓冲区
bool insert(buffer_item item)
{
	//获得锁
	WaitForSingleObject( empty, INFINITE );
	WaitForSingleObject( mutex, INFINITE );
	//插入数据 
	myBuffer[ in ] = item;
	in = (in + 1) % 5;
	//释放锁
	ReleaseSemaphore( mutex, 1, NULL );
	ReleaseSemaphore( full, 1, NULL );
	return true;
}
//从缓冲区移除一个数据
bool remove(buffer_item *item)
{	
	//获得锁
	WaitForSingleObject( full, INFINITE );
	WaitForSingleObject( mutex, INFINITE );
	//插入数据
	*item = myBuffer[out];
	out = (out + 1) % 5;
	//释放锁
	ReleaseSemaphore( mutex, 1, NULL );
	ReleaseSemaphore( empty, 1, NULL );
	return true;
}

//生产者
DWORD WINAPI producer(LPVOID  lpParam)
{
	int num = *(int*)lpParam;
	srand((unsigned)time(0));
	while (true)
	{
		//睡眠随机时间
		_sleep( rand() % 100 );
		//插入随机数
		int data = rand() % 100;
		//打印信息
		if ( insert(data) )
			//第num个消费者
			printf("Producer%d produced %d\n", num, data);
		else
			printf("Warning:produce error\n");
	} 	
}

//消费者
DWORD WINAPI consumer(LPVOID  lpParam)
{	
	int num = *(int*)lpParam;
	while (true)
	{
		//睡眠随机时间
		_sleep( rand() % 100 );
		//插入随机数
		int data = 0;
		//打印信息
		if ( remove(&data) )
			//第num个消费者
			printf("Consumer%d consumed %d\n", num, data);
		else
			printf("Warning:consumer error\n");
	}	
}

int main(void)
{
	freopen("windowOut.txt","w",stdout);
	//初始化信号量
	init();
	//创建 5 个生产者和5个消费者线程
	HANDLE producerThread[5] , consumerThread[5];
	int num[5] = {0 ,1, 2, 3, 4};
	for( int i = 0; i < 5; i++)
	{
		producerThread[i] = CreateThread(NULL,0,producer,&num[i],0,NULL); 
		consumerThread[i] = CreateThread(NULL,0,consumer,&num[i],0,NULL); 
	}
	//睡眠一段时间
	_sleep(5000);
	//退出线程
	for( int i = 0; i < 1; i++)
	{
		CloseHandle( producerThread[i] ); 
		CloseHandle( consumerThread[i] ); 
	}
	return 0;
}
