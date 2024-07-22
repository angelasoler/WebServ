
#include "Server.hpp"

Server::Server(int port) {
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	createSocket();
	configureSocket();
	bindSocket();
	listenSocket();
}

void	Server::createSocket(void)
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		handleError("Socket creation failed");
	}
}

void	handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

void	Server::configureSocket(void)
{
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void	Server::bindSocket(void)
{
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void	Server::listenSocket(void)
{
	if (listen(server_fd, 3) < 0)
	{
		handleError("Listen failed");
	}
	std::cout << "Pronto para conectar com clientes\nUtilize: '~$ \
	telnet localhost 8080' em outro terminal para testar!" << std::endl;

	struct pollfd server_poll_fd;
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN | POLLOUT;
	// server_poll_fd.revents = 0;
	poll_fds.push_back(server_poll_fd);
}

// void	Server::run(void)
// {
// 	loop.run(*this);
// }

Server::~Server(void) {}
