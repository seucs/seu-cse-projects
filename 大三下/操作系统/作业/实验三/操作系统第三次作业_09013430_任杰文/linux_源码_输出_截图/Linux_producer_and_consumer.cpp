#include <stdio.h> 
#include <unistd.h>
#include <semaphore.h>
#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
using namespace std;

//定义缓冲区数据类型
typedef int buffer_item;
//定义缓冲区大小
#define BUFFER_SIZE 5
//创建缓冲区
buffer_item myBuffer[BUFFER_SIZE];
//定义信号量
pthread_mutex_t mutex;
sem_t empty;          
sem_t full;      
//定义生产者、消费之缓冲区下标
int in = 0, out = 0;

//初始化函数
void init()
{	
	//初始化信号量
	pthread_mutex_init(&mutex,NULL); 
	sem_init(&empty,0,5);
	sem_init(&full,0,0);	
}

//插入一个数据到缓冲区
bool insert(buffer_item item)
{
	//获得锁
	sem_wait(&empty);
	pthread_mutex_lock(&mutex);
	//插入数据 
	myBuffer[ in ] = item;
	in = (in + 1) % 5;
	//释放锁
	pthread_mutex_unlock(&mutex);
	sem_post(&full);	
	return true;
}
//从缓冲区移除一个数据
bool remove(buffer_item *item)
{	
	//获得锁
	sem_wait(&full);
	pthread_mutex_lock(&mutex);
	//插入数据
	*item = myBuffer[out];
	out = (out + 1) % 5;
	//释放锁
	pthread_mutex_unlock(&mutex);
	sem_post(&empty);
	return true;
}
//生产者
void *producer(void *data)/////////////½«Ò»¸öËæ»úÊý·ÅÈë»º´æÇø£¬²¢ÏÔÊ¾ÌáÊ¾ÐÅÏ¢
{
	int num = *(int*)data;
	srand((unsigned)time(0));
	while (true)
	{
		//睡眠随机时间
		usleep( rand() % 100 );
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
void *consumer(void *data)
{	
	int num = *(int*)data;
	while (true)
	{
		//睡眠随机时间
		usleep( rand() % 100 );
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
	freopen("LinuxOut.txt","w",stdout);
	//初始化信号量
	init();
	//创建 5 个生产者和5个消费者线程
	pthread_t producerThread[5] , consumerThread[5];
	pthread_attr_t producerAttr[5], consumerAttr[5];
	int num[5] = {0 ,1, 2, 3, 4};
	for( int i = 0; i < 5; i++)
	{
		pthread_attr_init(&producerAttr[i]);
		pthread_attr_init(&consumerAttr[i]);
		pthread_create(&producerThread[i], &producerAttr[0], producer, &num[i]);
		pthread_create(&consumerThread[i], &consumerAttr[0], consumer, &num[i]);
	}
	//睡眠一段时间
	usleep(5000);
	//退出线程
	for( int i = 0; i < 1; i++)
	{
		pthread_cancel(producerThread[i]);
		pthread_cancel(consumerThread[i]);
	}
	return 0;
}
