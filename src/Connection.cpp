
#include "Connection.hpp"
#include "Server.hpp"

void	Connection::initSockets()
{
	Config	*config = Config::getInstance();

	nServers = config->servers.size();
	for (uint i = 0; i < nServers; i++)
	{
		struct pollfd	server_poll_fd;
		Server			tmp(config->servers[i]);

		servers.push_back(tmp);
		server_poll_fd.fd = servers[i].fd;
		server_poll_fd.events = POLLIN;
		poll_fds.push_back(server_poll_fd);
	}
}

Connection::Connection(void) {
	initSockets();
}

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
	clientServerConfig[new_socket] = refServer.config;
	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	client_poll_fd.revents = 0;
	poll_fds.push_back(client_poll_fd);
}

void	Connection::readClientRequest(int client_fd)
{
	if (request[client_fd].info.action == AWAIT_WRITE)
		return;
	request[client_fd].readRequest(client_fd);
	// std::cerr << "info.action: " << request[client_fd].info.action << "\n";
}

void	Connection::responseToClient(int client_fd)
{
	if (request[client_fd].info.action == AWAIT_READ)
		return;
	if (response.find(client_fd) == response.end()) {
		response[client_fd] = Response(request[client_fd].info, client_fd);
	}
	response[client_fd].treatActionAndResponse();
	request[client_fd].info.action = response[client_fd].requestInfo.action;
	request[client_fd].requestsText.clear();
}

void	Connection::treatRequest(int client_fd)
{
	if (request[client_fd].info.action == AWAIT_READ)
		return;
	if (request[client_fd].info.action == AWAIT_WRITE) {
		responseToClient(client_fd);
		return;
	}
	if (request[client_fd].requestVec.empty()) {
		request[client_fd].info.action = RESPONSE;
		return ;
	}
	request[client_fd].parseRequest(clientServerConfig[client_fd]);
	responseToClient(client_fd);
}

int	Connection::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

bool	Connection::eventIO(void)
{
	if (poll(poll_fds.data(), poll_fds.size(), -1) < 0) {
		if (errno == EINTR)
			return (false);
		handleError("Poll failed");
	}
	return (true);
}

void	Connection::verifyServerPollin(void)
{
	for (uint i = 0; i < nServers; i++) {
		if (poll_fds[i].revents & POLLIN) {
			connectNewClient(servers[i]);
		}
	}
}

void	Connection::cleanClient(int clientIdx)
{
	int client_fd = poll_fds[clientIdx].fd;

	if (request[client_fd].info.action == CLOSE) {
		std::cout << "\n ***** Close Client *****\n"
				<< "\tfd:\t"
				<< client_fd
				<< "\nclient correctly cleaned"
				<< std::endl;
		close(client_fd);
		request.erase(client_fd);
		response.erase(client_fd);
		poll_fds.erase(poll_fds.begin() + clientIdx);
	}
}

void	Connection::requestResponse(void)
{
	for (size_t clientIdx = nServers; clientIdx < poll_fds.size(); clientIdx++)
	{
		int client_fd = poll_fds[clientIdx].fd;

		if (poll_fds[clientIdx].revents & POLLIN) {
			readClientRequest(client_fd);
			cleanClient(clientIdx);
		}
		if (poll_fds[clientIdx].revents & POLLOUT) {
			treatRequest(client_fd);
			cleanClient(clientIdx);
		}
	}
}

void	Connection::cleanPollFds(void) {
	for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it == poll_fds.end(); it++)
		close(it->fd);

	for (std::vector<Server>::iterator it = servers.begin(); it != servers.end(); it++)
		close(it->fd);

	for (std::map<int, ServerConfig>::iterator it = clientServerConfig.begin(); it != clientServerConfig.end(); it++)
		close(it->first);
}
