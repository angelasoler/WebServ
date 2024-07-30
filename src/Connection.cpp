
#include "Connection.hpp"
#include "Server.hpp"

Connection::Connection(size_t nServers) : nServers(nServers) {}

Connection::Connection(void) {}

Connection::~Connection(void) {}

void	Connection::connectNewClient(Server &refServer)
{
	int	new_socket;
	int	addrlen = sizeof(refServer.address);
	struct pollfd client_poll_fd;

	if ((new_socket = accept(refServer.fd, (struct sockaddr*)&refServer.address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	std::cout << " ***** New client *****\n\t    fd: "
			<< new_socket
			<< "\nEphemeral Port: "
			<< ntohs(refServer.address.sin_port)
			<< std::endl;

	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}
	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	client_poll_fd.revents = 0;
	poll_fds.push_back(client_poll_fd);
}

void	Connection::readClientRequest(int client_fd, int clientIdx)
{
	if (request.readRequest(client_fd, requestsText)) {
		poll_fds.erase(poll_fds.begin() + clientIdx);
	}
}

void	Connection::treatRequest(int client_fd, int clientIdx) {
	e_httpMethodActions	action;
	std::string			text;

	if (!(!requestsText.empty() && \
		!requestsText[client_fd].empty()))
		return ;
	text = requestsText[client_fd];

	action = request.parseRequest(text);

	if (response.treatActionAndResponse(requestsText, client_fd, action))
	{
		poll_fds.erase(poll_fds.begin() + clientIdx);
		close(client_fd);
		requestsText.erase(client_fd);
	}

}

int	Connection::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool	Connection::eventIO()
{
	nPolls = poll_fds.size();
	if (poll(poll_fds.begin().base(), poll_fds.size(), -1) < 0) {
		if (errno == EINTR)
			return (false);
		handleError("Poll failed");
	}
	return (true);
}

void	Connection::verifyServerPollin(std::vector<Server> &servers)
{
	for (uint i = 0; i < servers.size(); i++) {
		if (poll_fds[i].revents & POLLIN) {
			std::cout << "verifyServerPollin: "
				<< servers.size()
				<< std::endl;
			connectNewClient(servers[i]);
		}
	}
}

void	Connection::requestResponse(void)
{
	for (size_t clientIdx = nServers; clientIdx < nPolls; clientIdx++)
	{
		if (poll_fds[clientIdx].revents & POLLIN) {
			readClientRequest(poll_fds[clientIdx].fd, clientIdx);
		}
		if (poll_fds[clientIdx].revents & POLLOUT) {
			treatRequest(poll_fds[clientIdx].fd, clientIdx);
		}
	}
}

void	Connection::cleanPollFds(void) {
	for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it == poll_fds.end(); it++)
		close(it->fd);
}
