#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>

//读回调
void read_cb(struct bufferevent* bev, void* arg)
{
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    std::cout<<"read buff: "<<buf;
    //发送数据--往缓冲区中写数据
    const char* ptr = "read cb : already read !\n";
    //会在常量区定义一个string类型的常量，并返回一个常量指针指向字符串
    //常量指针是const类型的，所以必须使用const类型指针（c中可以使用非const指针）
    bufferevent_write(bev, ptr, strlen(ptr));
    //ptr是指针4字节,64位系统8字节,如果使用sizeof只能返回8字节（read cb ）
    //buf虽然是数组首元素地址，但是数组可以理解为buf+1024（数组长度），所以sizeof的结果是1024
    std::cout<<"already write for client !"<<std::endl;
}
//写回调
void write_cb(struct bufferevent* bev, void* arg)
{
    std::cout<<"write_cb : already write !"<<std::endl;
}
//事件回调
void event_cb(struct bufferevent* bev, short events, void* arg)
{
    if(events & BEV_EVENT_EOF)
    {
        std::cout<<" connect closed " <<std::endl;
    }
    else if(events & BEV_EVENT_ERROR)
    {
        std::cout<<" other error " <<std::endl;
    }
    bufferevent_free(bev);
}
//连接完成后对应的操作
void listen_cb(struct  evconnlistener* listen,
                evutil_socket_t fd,
                struct  sockaddr* addr,
                int len, void* ptr)
{
    //得到传进来的event_base
    struct event_base* base = (struct event_base*)ptr;
    //先接收数据，在发送数据
    //将fd封装为bufferevent
    struct  bufferevent* bev = NULL;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    //给bufferevent读写缓冲区设置相应的回调函数
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
    //设置读缓冲可用，写缓冲默认是可用的
    bufferevent_enable(bev, EV_READ);   
}
int main()
{
    //创建事件处理框架
    struct event_base* base = event_base_new();
    //init server info
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(80);
    serv.sin_addr.s_addr = htonl(INADDR_ANY);
    //创建监听套接字
    //绑定
    //监听
    //等待并接收了连接
    struct evconnlistener* listen = NULL;
    //当有新的连接过来时，listen_cb 会被调用
    listen = evconnlistener_new_bind(base, listen_cb, base, LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, 
                                    (struct sockaddr*)&(serv) , sizeof(serv));
    //开始事件循环
    event_base_dispatch(base);    
    
    //释放资源
    evconnlistener_free(listen);
    event_base_free(base);
    return 0;
}