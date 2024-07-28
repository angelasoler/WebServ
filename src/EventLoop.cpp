#include "EventLoop.hpp"
#include <sys/epoll.h>


volatile sig_atomic_t	running = true;
uint					N_SERVERS;

EventLoop::EventLoop() : connection(new Connection()) {}

EventLoop::~EventLoop(void) {
	delete connection;
}


// TO-DO
// [_] sighandler class
// [_] usar pipe para acordar poll e poder limpar os fds corretamente
// void	stopLoop(int sig)
// {
// 	(void) sig;
// 	stop = true;
// 	sleep(1);
// }
void stopRunning(int signal) {
	(void) signal;
	running = false;
}

void setup_signal_handlers() {
	struct sigaction sa;
	sa.sa_handler = stopRunning;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}
#include <errno.h>
// TO-DO
// [_] Make a state machine to handle each event
void	EventLoop::run(std::vector<Server> &servers)
{
	size_t	event_count;

	int epoll_fd;

	epoll_fd = epoll_create1(0);
	if (epoll_fd == -1) {
		handleError("epoll_create1 failed");
		return;
	}

	for (uint i = 0; i < N_SERVERS; i++) {
		if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, servers[i].fd, &connection->poll_fds[i]) == -1) {
			handleError("epoll_ctl failed");
			return;
		}
	}
	setup_signal_handlers();
	while (running)
	{
		event_count = epoll_wait(epoll_fd, connection->poll_fds.data(), connection->poll_fds.size(), -1);
		if (event_count < 0) {
			if (errno == EINTR) // epoll_wait foi interrompido por um sinal
				continue ;
			handleError("Poll failed");
		}
		for (uint i = 0; i < N_SERVERS; i++) {
			if (connection->poll_fds[i].events & POLLIN)
				connection->connectNewClient(servers[i]);
		}
		for (size_t clientIdx = N_SERVERS; clientIdx < event_count; clientIdx++)
		{
			if (connection->poll_fds[clientIdx].events & POLLIN) {
				connection->readClientRequest(connection->poll_fds[clientIdx].data.fd, clientIdx);
			}
			if (connection->poll_fds[clientIdx].data.fd & POLLOUT) {
				connection->treatRequest(connection->poll_fds[clientIdx].data.fd, clientIdx);
			}
		}
	}
}
