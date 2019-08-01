#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <event2/event.h>
#include <event2/listener.h>
//回调函数
void read_callback(evutil_socket_t fd, short what, void* arg)
{
    sleep(1);
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf)); 
    std::cout<<"data len = "<<len<<"  buf  = "<<buf<<std::endl;
    std::cout<<"read event: "<< ((what & EV_READ) ? ("yes"):("no")) <<std::endl;
}
int main()
{
    unlink("myfifo");
    mkfifo("myfifo", 0644);
    //命名管道必须的手动打开
    int fd = open("myfifo",O_RDONLY | O_NONBLOCK);
    if(fd < 0)
    {
        std::cout<<"open error"<<std::endl;
        exit(-1);
    }
    //创建框架
    struct event_base* base = NULL;
    base = event_base_new();
    //创建事件
    struct event* event;
    event = event_new(base, fd, EV_READ | EV_PERSIST, read_callback, NULL);
    //添加事件
    event_add(event,NULL);
    //事件循环
    event_base_dispatch(base);
    //释放资源
    event_free(event);
    event_base_free(base);
    close(fd);

    return 0;
}