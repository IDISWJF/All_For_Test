#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
//读回调
void read_cb(struct bufferevent* bev, void* arg)
{
    char buf[1024] = {0};
    bufferevent_read(bev, buf, sizeof(buf));
    std::cout<<"read buff: "<<buf<<std::endl;
}
//写回调
void write_cb(struct bufferevent* bev, void* arg)
{  
    //std::cout<<"write_cb : already write !"<<std::endl;
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
    else if (events & BEV_EVENT_CONNECTED)
    {
        std::cout<<"connnect success"<<std::endl;   
        return ;     
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
  
    struct  bufferevent* bev = NULL;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
   
}
void read_terminal(int fd, short what, void* arg)
{
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf));
    //std::cout<<len<<std::endl;测试因为read会把回车读走，所以输出会多一个换行
    struct bufferevent* bev = (struct bufferevent*)arg;
    bufferevent_write(bev, buf, len);
}
//接收终端输入，将数据发送给server
int main()
{
    //创建事件处理框架
    struct event_base* base = event_base_new();
    //init server info
    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(1234);
    inet_pton(AF_INET, "127.0.0.1", &serv.sin_addr.s_addr);
    //创建事件
    //连接服务器--fd
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    //fd绑定bufferevent事件
    struct bufferevent* bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    //连接服务器
    bufferevent_socket_connect(bev, (struct sockaddr*)&serv, sizeof(serv));
    //设置回调
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, NULL);
    bufferevent_enable(bev, EV_READ);
    //接收键盘输入，创建一个新事件
    struct event* ev = event_new(base, STDIN_FILENO, EV_READ | EV_PERSIST,read_terminal, bev);
    //添加事件到框架
    event_add(ev, NULL);
    //开始事件循环
    event_base_dispatch(base);    
    
    //释放资源
    event_base_free(base);
    return 0;
}