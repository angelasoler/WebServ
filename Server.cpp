
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

Server::~Server(void) {}

// [] Class Response
void	Server::ReponseClient(int client_fd)
{
	send(client_fd, "Hello\n", 6, 0);
}

void	Server::createSocket(void)
{
	struct pollfd	server_poll_fd;
	int				server_fd;

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		handleError("Socket creation failed");
	}
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN | POLLOUT;
	poll_fds.push_back(server_poll_fd);
}

int		Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// [] Class Connection
void	Server::connectNewClient()
{
	int	new_socket;
	int	addrlen = sizeof(address);

	if ((new_socket = accept(poll_fds[0].fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	std::cout
	<< " ***** Nova conexão *****\nfd:\t" << new_socket 
	<< "\nporta: " << address.sin_port 
	<< std::endl;

	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}

	struct pollfd client_poll_fd;

	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	client_poll_fd.revents = 0;
	poll_fds.push_back(client_poll_fd);
}

// [] Class request
void	Server::treatRequest(int client_fd, int clientIdx)
{
	std::string			text;
	e_httpMethodActions	action;

	text = request[client_fd];
	if (!std::strncmp(text.c_str(), "GET", 3)
		|| !std::strncmp(text.c_str(), "POST", 4)
		|| !std::strncmp(text.c_str(), "DELETE", 6))
	{
		std::cout << "RESPONSE\n" << std::endl; //ao responder limpar fd
		action = RESPONSE;
	}
	else {
		std::cout << "CLOSE\n" << std::endl;
		action = CLOSE;
	}
	if (!request.empty() && !request[client_fd].empty())
	{
		switch (action)
		{
			case RESPONSE:
				ReponseClient(client_fd);
				break;
			case CLOSE:
				request.erase(request.find(client_fd));
				break;
		}
		poll_fds.erase(poll_fds.begin() + clientIdx);
		close(client_fd);
		request.erase(client_fd);
	}
}

// [] Class Connection
int		Server::getRequest(int client_fd, int clientIdx)
{
	char	buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = '\0';
	if (bytes_read < 0)
	{
		handleError("Read failed");
	}
	else if (bytes_read == 0)
	{
		std::cout << "Conexão fechada pelo cliente" << std::endl;
		close(client_fd);
		poll_fds.erase(poll_fds.begin() + clientIdx);
		return (1);
	}
	else
	{
		request[client_fd] += buffer;
		std::cout
		// << "Request: "
		// << buffer
		<< std::endl;
	}
	return (0);
}

void	Server::configureSocket(void)
{
	int opt = 1;
	if (setsockopt(poll_fds[0].fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void	Server::bindSocket(void)
{
	if (bind(poll_fds[0].fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void	Server::listenSocket(void)
{
	if (listen(poll_fds[0].fd, 3) < 0)
	{
		handleError("Listen failed");
	}
	std::cout << "Pronto para conectar com clientes\nUtilize: '~$ \
	telnet localhost 8080' em outro terminal para testar!" << std::endl;
}

void	handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}
