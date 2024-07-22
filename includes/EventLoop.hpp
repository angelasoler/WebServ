
#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

# include <iostream>
# include <arpa/inet.h>
# include <poll.h>
# include <map>
# include <algorithm>
# include <cstring>
#include "Server.hpp"

class EventLoop
{
	private:
		Server	&refServer;

	public:
		EventLoop(Server &server);
		~EventLoop(void);

		void	run();
};

#endif /* EVENTLOOP_HPP */
