//信号量：生产者消费者模型
//锁机制是有限制的,锁只有两种状态，对于互斥访问一个全局变量还可以
//但是对于其他临界资源,比如多台打印机就不行
//这时就需要用到信号量，这个计数器表示空闲的临界资源数量

    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <pthread.h>
    #include <semaphore.h>
     
 sem_t sem ;
 //生产者
 void *producer1(void *)
 {
     while(1)
     {
         sleep(1);
         printf("生产者1\n");
         sem_post(&sem);//++
     }
 }
 void *producer2(void *)
 {
     while(1)
     {
         sleep(1);
         printf("生产者2\n");
         sem_post(&sem);//++
     }
 }
 //消费者
 void *consumer1(void *)
 {
 while(1)
     {
         sleep(1);
         printf("消费者1\n");
         sem_wait(&sem);//--
     }
 }
 void *consumer2(void *)
 {
 while(1)
     {
         sleep(1);
         printf("消费者2\n");
         sem_wait(&sem);//--
     }
 }
 int main()
 {
     pthread_t p1,p2,p3,p4;
    //第一个参数代表初始化的信号量 第二:0用于线程间，
    //非0用于进程间 第三参数：信号量的初始化计数(代表资源开始有几份可用)
     sem_init(&sem, 0, 0);
     pthread_create(&p1,NULL,&producer1,NULL);
     pthread_create(&p1,NULL,&producer2,NULL);
     pthread_create(&p2,NULL,&consumer1,NULL);
     pthread_create(&p3,NULL,&consumer2,NULL);
     pthread_join(p1,NULL);
     pthread_join(p2,NULL);
     pthread_join(p3,NULL);
     pthread_join(p4,NULL);
     sem_destroy(&sem);
     return 0;
 }