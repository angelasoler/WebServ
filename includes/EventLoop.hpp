
#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

# include <iostream>
# include <arpa/inet.h>
# include <poll.h>
# include <map>
# include <algorithm>
# include <cstring>
#include "Connection.hpp"

extern uint N_SERVERS;

class EventLoop
{
	public:
		Connection			*connection;

		EventLoop();
		~EventLoop(void);

		void	run(std::vector<Server> &servers);
};

#endif /* EVENTLOOP_HPP */
