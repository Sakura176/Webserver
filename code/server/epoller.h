#ifndef EPOLLER_H
#define EPOLLER_H

#include <sys/epoll.h> //epoll_ctl()
#include <fcntl.h>  // fcntl()
#include <unistd.h> // close()
#include <assert.h> // close()
#include <vector>
#include <errno.h>

/**
 * IO复用技术 Epoll 类
*/
class Epoller {
public:
	// 避免进行隐式类型转换
	explicit Epoller(int maxEvent = 1024);

	~Epoller();

	bool AddFd(int fd, uint32_t events);

	bool ModFd(int fd, uint32_t events);

	bool DelFd(int fd);

	int Wait(int timeoutMs = -1);

	int GetEventFd(size_t i) const;

	uint32_t GetEvents(size_t i) const;
		
private:
	int epollFd_;

	/**
	 *  struct epoll_event
		{
			uint32_t events;  // epoll注册的事件	
			epoll_data_t data;	
		} __EPOLL_PACKED;
	*/
	std::vector<struct epoll_event> events_;    
};

#endif //EPOLLER_H