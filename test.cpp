#include <iostream>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <string>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <string.h>
#include <fstream>
#define CAN_FRAME_LEN sizeof(struct can_frame)
////////
class SocketCan {
	public:
		//wjf
		SocketCan();
		~SocketCan();
		bool Read(struct can_frame &can_msg);
		bool Write(const struct can_frame &can_msg);
		int Connect();
		int Reconnect();
		std::string can_name_;
		std::mutex lock_;
		int can_sockfd_ ;

		uint64_t current_frame_timestamp_sec_ {0};
		uint64_t current_frame_timestamp_nsec_ {0};
};

SocketCan::SocketCan() {
	std::string param = "device=vcan0";
	std::size_t nop = param.find("=");
	if (nop != std::string::npos){
		can_name_ = param.substr(nop + 1);
	}
	can_sockfd_ = Connect();
}

SocketCan::~SocketCan() {
	if (can_sockfd_ > 0){
		close(can_sockfd_);
	}
}

int SocketCan::Connect() {
	struct sockaddr_can addr;
	struct ifreq ifr;
memset(&ifr,0,sizeof(ifr));
	int can_sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);
	if (-1 == can_sockfd){
		std::cout << "Socket failed! error:" << strerror(errno)
			<< std::endl;
		exit(EXIT_FAILURE);
	}

	can_name_.copy(ifr.ifr_name, can_name_.size());
	if ( -1 == ioctl(can_sockfd, SIOCGIFINDEX, &ifr)){
		std::cout << "Ioctl failed! error" << strerror(errno)
			<< std::endl;
	}

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;
	if (-1 == bind(can_sockfd, (struct sockaddr *)&addr, sizeof(addr))){
		std::cout << "Bind failed! error:" << strerror(errno)
			<< std::endl;
		exit(EXIT_FAILURE);    
	}

	return can_sockfd;
}

int SocketCan::Reconnect() {
	int can_sockfd = can_sockfd_;
	std::unique_lock<std::mutex> lk(lock_);
	if (can_sockfd != can_sockfd_) {
		return 0; 
	}
	if (can_sockfd_ > 0) {
		close(can_sockfd_);
	}

	can_sockfd_ = Connect();

	return 0;
}

bool SocketCan::Read(struct can_frame &can_msg) {
	int read_len = read(can_sockfd_, &can_msg, CAN_FRAME_LEN);
	if(read_len > 0)
	{
		return true;
	}
	else if (read_len == 0) {
		std::cout << "Connection closed by peer." << std::endl;
		Reconnect();
		return true;
	} 
	else {

		std::cout << "Read error: " << strerror(errno) << std::endl;
		Reconnect();
		return false;
	}
}

bool SocketCan::Write(const struct can_frame &can_msg) {
	int write_len = write(can_sockfd_, &can_msg, CAN_FRAME_LEN);
	if (write_len < 0) {
		std::cout << "Write error: " << strerror(errno) << std::endl;
		Reconnect();
		return false;
	}
	return true;
}



int main(int argc, char const *argv[]){
	SocketCan sc;
#ifdef SEND
	char buf[100] = {};
	 memset(buf,-1,sizeof(buf));
	std::ifstream file("raw_data.txt");
	int nbytes = 0;
	while(!file.eof())
	{
		usleep(2000);
		file.getline(buf,100);

		struct can_frame frame;
		sscanf(buf,"%d---%d---%d-%d-%d-%d-%d-%d-%d-%d",&frame.can_id,&frame.can_dlc           \
				,&frame.data[0],&frame.data[1],&frame.data[2] \
				,&frame.data[3],&frame.data[4],&frame.data[5] \
				,&frame.data[6],&frame.data[7]);
		std::cout<<frame.can_id<<"---"<<(int)frame.can_dlc<<"---"<<(int)frame.data[0] \
			<<"-"<<(int)frame.data[1]<<"-"<<(int)frame.data[2]<<"-"<<(int)frame.data[3]<<"-"<<(int)frame.data[4] 
			<<"-"<<(int)frame.data[5]<<"-"<<(int)frame.data[6]<<"-"<<(int)frame.data[7]<<std::endl;
		sc.Write(frame); 

	}
#endif


#ifdef RECV
	while (true)
	{
		struct can_frame frame ;
		sc.Read(frame);
		std::cout<<frame.can_id<<"---"<<(int)frame.can_dlc<<"---"<<(int)frame.data[0] \
			<<"-"<<(int)frame.data[1]<<"-"<<(int)frame.data[2]<<"-"<<(int)frame.data[3]<<"-"<<(int)frame.data[4]
			<<"-"<<(int)frame.data[5]<<"-"<<(int)frame.data[6]<<"-"<<(int)frame.data[7]<<std::endl;
	}
#endif
	return 0;
}
