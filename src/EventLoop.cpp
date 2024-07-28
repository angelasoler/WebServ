#include "EventLoop.hpp"
#include <csignal>
#include <vector>
#include <algorithm>


volatile sig_atomic_t	stop = false;
uint					N_SERVERS;

EventLoop::EventLoop() : connection(new Connection()) {}

EventLoop::~EventLoop(void) {
	delete connection;
}


// TO-DO
// [_] sighandler class
// [_] usar pipe para acordar poll e poder limpar os fds corretamente
void	stopLoop(int sig)
{
	(void) sig;
	stop = true;
	sleep(1);
}

// TO-DO
// [_] Make a state machine to handle each event
void	EventLoop::run(std::vector<Server> &servers)
{
	size_t	nPolls;

	signal(SIGINT, stopLoop);
	while (!stop)
	{
		nPolls = connection->poll_fds.size();
		if (poll(connection->poll_fds.begin().base(), nPolls, -1) < 0) {
			handleError("Poll failed");
		}
		for (uint i = 0; i < N_SERVERS; i++) {
			if (connection->poll_fds[i].revents & POLLIN) {
				connection->connectNewClient(servers[i]);
			}
		}
		for (size_t clientIdx = N_SERVERS; clientIdx < nPolls; clientIdx++)
		{
			if (connection->poll_fds[clientIdx].revents & POLLIN) {
				connection->readClientRequest(connection->poll_fds[clientIdx].fd, clientIdx);
			}
			if (connection->poll_fds[clientIdx].revents & POLLOUT) {
				connection->treatRequest(connection->poll_fds[clientIdx].fd, clientIdx);
			}
		}
	}
	for (std::vector<struct pollfd>::iterator it = connection->poll_fds.begin(); it == connection->poll_fds.end(); it++)
		close(it->fd);
}
