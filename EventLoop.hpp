
#ifndef EVENTLOOP_HPP
#define EVENTLOOP_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# include <iostream>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <map>
# include <algorithm>
# include <cstring>
#include "Server.hpp"

typedef enum
{
	RESPONSE,
	CLOSE
}	e_methodActions;

class EventLoop
{
	private:
		Server						&refServer;
		std::map<int, std::string>	message;

	public:
		EventLoop(Server &server);
		~EventLoop(void);

		void	run();

		void	acceptConnection(void);
		int		handleClient(int client_fd, int poll_index);
		int		setNonBlocking(int fd);
		void	answer_it(int client_fd);

		int		treatHTTPMethod(std::string text);
};

#endif /* EVENTLOOP_HPP */
