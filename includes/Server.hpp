
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

#define N_SERVERS 1

class Connection;

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
		std::vector<struct pollfd>	poll_fds;//leak
		std::map<int, std::string>	requestText;
		Connection					*connection;

		void	connectNewClient(void);
		void	getRequest(int client_fd, int clientIdx);
		void	treatRequest(int client_fd, int clientIdx);
		Server(int port);
		~Server(void);
};

void	handleError(const std::string& msg);

#endif /* SERVER_HPP */
