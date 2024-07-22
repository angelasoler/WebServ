
#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <cstdio>
# include <cstdlib>
# include <cstring>
# include <unistd.h>
# include <arpa/inet.h>
# include <poll.h>
# include <vector>
# include <fcntl.h>

# include <algorithm>
# include <map>

# ifndef PORT
#  define PORT 8080
# endif

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

typedef enum
{
	RESPONSE,
	REVENT,
	CLOSE
}	e_methodActions;

class Server
{
	public:
		Server(int port);
		~Server(void);
		void run(void);

	private:
		int							server_fd;
		struct sockaddr_in 			address;
		int							addrlen;
		char						buffer[BUFFER_SIZE + 1];
		std::vector<struct pollfd>	poll_fds;
		std::map<int, std::string>	message;

		void	createSocket(void);
		void	configureSocket(void);
		void	bindSocket(void);
		void	listenSocket(void);
		void	acceptConnection(void);
		int		handleClient(size_t i);
		void	handleError(const std::string& msg);
		int		setNonBlocking(int fd);
		void	answer_it(size_t i);

		int		getActionNumber(std::string text);


};

#endif
