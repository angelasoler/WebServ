
#ifndef SERVER_HPP
#define SERVER_HPP

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

#include "Config.hpp"

class Server
{
	private:
		int		createSocket(void);
		void	configureSocket(void);
		void	bindSocket(void);
		void	listenSocket(void);
		int		setNonBlocking(int client_fd);

	public:
		int							fd;
		struct sockaddr_in 			address;

		Server(const Server &cpy);
		Server(ServerConfig &server);
		~Server(void);
		Server &operator=(const Server &cpy);
};

void	handleError(const std::string& msg);

#endif /* SERVER_HPP */
