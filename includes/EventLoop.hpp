
#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

# include <iostream>
# include <arpa/inet.h>
# include <poll.h>
# include <map>
# include <algorithm>
# include <cstring>
# include <csignal>
# include "Connection.hpp"

class EventLoop
{
	private:
		bool	verifyEvents(void);
		void	acceptConnection(std::vector<Server> &servers);
		void	manageClientIO(void);
	public:
		Connection	*connection;

		EventLoop();
		EventLoop(const size_t nServers);
		~EventLoop(void);

		void	run(std::vector<Server> &servers);
};

#endif /* EVENTLOOP_HPP */
