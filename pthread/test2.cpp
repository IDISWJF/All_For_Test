#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
int number = 10000000;
int tmp = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void *count(void*)
{
	if(pthread_mutex_lock(&mutex) != 0)
	{
		perror("mutex");
		exit(EXIT_FAILURE);;
	}
	while(number > 0)
	{

		tmp++;
		number--;

		// sleep(1);
		// std::cout<<"id : "<<pthread_self()<<" number :"
		// <<"tmp : "<<number<<" : "<<tmp<<std::endl;
	}
	if(pthread_mutex_unlock(&mutex) != 0)
	{
		perror("mutex");
		exit(EXIT_FAILURE);
	}
}
int main()
{
	pthread_t p1;
	pthread_t p2;
	pthread_t p3;

	pthread_create(&p1,NULL,count,NULL);
	pthread_create(&p2,NULL,count,NULL);
	pthread_create(&p3,NULL,count,NULL);

	pthread_join(p1,NULL);
	pthread_join(p2,NULL);
	pthread_join(p3,NULL);

	std::cout<<" number :"<<number<<std::endl;
	std::cout<<" tmp :"<<tmp<<std::endl;
	return 0;
}
