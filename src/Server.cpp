
#include "Server.hpp"
#include "Connection.hpp"

Server::~Server(void) {}

Server::Server(ServerConfig &config) : config(config) {
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(config.port);
	address.sin_family = AF_INET;
	fd = createSocket();
	configureSocket();
	bindSocket();
	listenSocket();
}

Server::Server(const Server &cpy)  : config(cpy.config)
{
	*this = cpy;
}

Server &Server::operator=(const Server &cpy)
{
	if (this != &cpy) {
		config = cpy.config;
		address = cpy.address;
		fd = cpy.fd;
	}
	return (*this);
}

int	Server::createSocket(void)
{
	int	server_fd;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		handleError("Socket creation failed");
	}
	return (server_fd);
}

void	Server::configureSocket(void)
{
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void	Server::bindSocket(void)
{
	if (bind(fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void	Server::listenSocket(void)
{
	if (listen(fd, 3) < 0)
	{
		handleError("Listen failed");
	}
	std::cout << "Ready to connect with clients, "
			<< ntohs(address.sin_port)
			<< " port " 
			<< " is listeting!" 
			<< std::endl;
}

void	handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}
