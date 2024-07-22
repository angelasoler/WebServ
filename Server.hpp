
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

# include <algorithm>
# include <map>

class Server
{
	public:
		Server(int port);
		~Server(void);
		void run(void);

		int							server_fd;
		struct sockaddr_in 			address;
		std::vector<struct pollfd>	poll_fds;
	private:

		void						createSocket(void);
		void						configureSocket(void);
		void						bindSocket(void);
		void						listenSocket(void);
};

void	handleError(const std::string& msg);

#endif /* SERVER_HPP */
