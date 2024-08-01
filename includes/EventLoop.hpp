
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
		void	acceptConnection(void);
		void	manageClientIO(void);
	public:
		Connection	*connection;

		EventLoop();
		~EventLoop(void);

		void	run(void);
};

#endif /* EVENTLOOP_HPP */
