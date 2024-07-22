
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
	REVENT,
	CLOSE
}	e_methodActions;

class EventLoop
{
	private:
		std::map<int, std::string>	message;
		Server						&refServer;
		char						buffer[BUFFER_SIZE + 1]; //arrancar + 1

	public:
		EventLoop(Server &server);
		~EventLoop(void);

		void	run();

		void	acceptConnection(void);
		int		handleClient(int client_fd);
		int		setNonBlocking(int fd);
		void	answer_it(int client_fd);

		int		getActionNumber(std::string text);
};

#endif /* EVENTLOOP_HPP */
