#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

class Server
{
	public:
		Server(int port);
		~Server();
		void run();

	private:
		int                         server_fd;
		struct sockaddr_in          address;
		int                         addrlen;
		char                        buffer[BUFFER_SIZE];
		std::vector<struct pollfd>  poll_fds;

		void createSocket();
		void configureSocket();
		void bindSocket();
		void listenSocket();
		void acceptConnection();
		void handleClient(size_t i);
		void handleError(const std::string& msg);
		int setNonBlocking(int fd);
};
