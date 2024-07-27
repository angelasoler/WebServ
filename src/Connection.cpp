
#include "Connection.hpp"
#include "Server.hpp"


Connection::Connection(Server &server) : refServer(server) {}

Connection::Connection(const Connection &cpy) : refServer(cpy.refServer) {}

Connection::~Connection(void) {}

void	Connection::newClient(void)
{
	int	new_socket;
	int	addrlen = sizeof(refServer.address);

	if ((new_socket = accept(refServer.poll_fds[0].fd, (struct sockaddr*)&refServer.address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	std::cout
	<< " ***** Nova conexão *****\nfd:\t" << new_socket 
	<< "\nporta: " << refServer.address.sin_port 
	<< std::endl;

	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}

	struct pollfd client_poll_fd;

	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	client_poll_fd.revents = 0;
	refServer.poll_fds.push_back(client_poll_fd);
}

void	Connection::readClientRequest(int client_fd, int clientIdx)
{
	if (request.readRequest(client_fd, refServer.requestText))
		refServer.poll_fds.erase(refServer.poll_fds.begin() + clientIdx);
}

void	Connection::breakRequest(int client_fd, int clientIdx) {
	e_httpMethodActions	action;
	std::string			text;

	text = refServer.requestText[client_fd];

	action = request.parseRequest(text);

	if (response.treatActionAndResponse(refServer.requestText, client_fd, action))
	{
		refServer.poll_fds.erase(refServer.poll_fds.begin() + clientIdx);
		close(client_fd);
		refServer.requestText.erase(client_fd);
	}

}

int	Connection::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
