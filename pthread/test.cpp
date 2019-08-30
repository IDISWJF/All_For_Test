#include<iostream>
#include<unistd.h>
#include<pthread.h>

void *run(void *arg)
{
	int count = 5;
	while(count--)
	{
		std::cout<<"pid : "<<pthread_self()<<"   :   ";
		std::cout<<"count : "<<count<<std::endl;
		sleep(1);
	}
}
int main()
{
	pthread_t t1;
	pthread_t t2;
	pthread_t t3;
	pthread_create(&t1, NULL,run,NULL);
	pthread_create(&t2, NULL,run,NULL);
	pthread_create(&t3, NULL,run,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);

	std::cout<<"end"<<std::endl;

	return 0;
}
