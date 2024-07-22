
#include "EventLoop.hpp"

EventLoop::EventLoop(Server &server) : refServer(server) {}

EventLoop::~EventLoop(void) {}

// TO-DO
// [_] Delegate to Response class
void	EventLoop::answer_it(int client_fd)
{
	send(client_fd, "Hello\n", 6, 0);
	close(client_fd);
	message.erase(message.find(client_fd));
	for (std::vector<struct pollfd>::iterator it = refServer.poll_fds.begin(); it != refServer.poll_fds.end(); it++)
	{
		if (it->fd == client_fd)
		{
			refServer.poll_fds.erase(it);
			break;
		}
	}
}

// TO-DO
// [_] Delegate new sockets poll to Client class
void	EventLoop::acceptConnection(void)
{
	int	new_socket;
	int	addrlen = sizeof(refServer.address);

	if ((new_socket = accept(refServer.server_fd, (struct sockaddr*)&refServer.address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	std::cout << "Nova conexão, socket fd é " << new_socket << ", ip é: " << ((refServer.address.sin_addr.s_addr << 24) >> 24)
		<< "." << ((refServer.address.sin_addr.s_addr << 16) >> 24)
		<< "." << ((refServer.address.sin_addr.s_addr << 8) >> 24)
		<< "." << ((refServer.address.sin_addr.s_addr << 0) >> 24) << ", porta: " << refServer.address.sin_port << std::endl;

	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}

	struct pollfd client_poll_fd;

	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	refServer.poll_fds.push_back(client_poll_fd);
}

// TO-DO
// [_] Delegate to Client class
int		EventLoop::handleClient(int client_fd)
{
	memset(buffer, 0, BUFFER_SIZE + 1);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = 0;
	if (bytes_read < 0)
	{
		handleError("Read failed");
	}
	else
	{
		message[client_fd] += buffer;
		std::cout << "Recebido do fd " << client_fd << ":"
		// << buffer
		<< std::endl;
	}
	return (0);
}

// TO-DO
// [_] Delegate to Client class
int		EventLoop::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		return -1;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// TO-DO
// [_] Delegate to Request class
int		EventLoop::getActionNumber(std::string text)
{
	if (!std::strncmp(text.c_str(), "GET", 3)
		|| !std::strncmp(text.c_str(), "POST", 4)
		|| !std::strncmp(text.c_str(), "DELETE", 6))
	{
		std::cout << "RESPONSE" << std::endl;
		return (RESPONSE);
	}
	else if (text.size() > 50) {
		std::cout << "CLOSE" << std::endl;
		return (CLOSE);
	}
	return (REVENT);
}

// TO-DO
// [_] Make a state machine to handle each event
void	EventLoop::run()
{
	while (true)
	{
		int poll_count = poll(refServer.poll_fds.begin().base(), refServer.poll_fds.size(), -1);
		if (poll_count < 0)
		{
			handleError("Poll failed");
		}

		if (refServer.poll_fds[0].revents & POLLIN) {
			std::cout << "\n********** acceptConnection *************\n" << std::endl;
			acceptConnection();
		}
		for (size_t i = 1; i < refServer.poll_fds.size(); ++i)
		{
			if (refServer.poll_fds[i].revents & POLLIN)
				handleClient(refServer.poll_fds[i].fd);
			if (refServer.poll_fds[i].revents & POLLOUT)
			{
				if (!message.empty() && !message.find(refServer.poll_fds[i].fd)->second.empty())
				{
					std::cout << "\n****** getAllText *****\n" << std::endl;
					switch (getActionNumber(message.find(refServer.poll_fds[i].fd)->second))
					{
						std::cout << "\n****** getActionNumber *****\n" << std::endl;
						case RESPONSE:
							answer_it(refServer.poll_fds[i].fd);
							break;
						case REVENT:
							break;
						case CLOSE:
							message.erase(message.find(refServer.poll_fds[i].fd));
							close(refServer.poll_fds[i].fd);
							refServer.poll_fds.erase(refServer.poll_fds.begin() + i);
							break;
					}
				}
			}
		}
	}
}