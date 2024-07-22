
#ifndef SERVER_HPP
#define SERVER_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# include <iostream>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <unistd.h>
# include <vector>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>

# include <algorithm>
# include <map>
# include "Connection.hpp"

typedef enum
{
	RESPONSE,
	CLOSE
}	e_httpMethodActions;

class Server
{
	private:
		void	createSocket(void);
		void	configureSocket(void);
		void	bindSocket(void);
		void	listenSocket(void);
		int		setNonBlocking(int client_fd);

	public:
		struct sockaddr_in 			address;
		std::vector<struct pollfd>	poll_fds;
		std::map<int, std::string>	request;

		void	connectNewClient(void);
		int		getRequest(int client_fd, int clientIdx);
		void	ReponseClient(int client_fd);
		void	treatRequest(int client_fd, int clientIdx);
		Server(int port);
		~Server(void);
};

void	handleError(const std::string& msg);

#endif /* SERVER_HPP */
