#include "EventLoop.hpp"

volatile sig_atomic_t	running = true;

EventLoop::EventLoop() {}

EventLoop::EventLoop(const size_t nServers) : connection(new Connection(nServers)) {}

EventLoop::~EventLoop(void) {
	delete connection;
}

// TO-DO
// [_] sighandler class
void stopLoop(int signal) {
	(void) signal;
	running = false;
}

void setup_signal_handlers() {
	struct sigaction sa;
	sa.sa_handler = stopLoop;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

bool	EventLoop::verifyEvents(void)
{
	return (connection->eventIO());
}

void	EventLoop::acceptConnection(std::vector<Server> &servers)
{
	connection->verifyServerPollin(servers);
}

void	EventLoop::manageClientIO(void)
{
	connection->requestResponse();
}

void	EventLoop::run(std::vector<Server> &servers)
{
	setup_signal_handlers();
	while (running)
	{
		if (!verifyEvents())
			continue ;
		acceptConnection(servers);
		manageClientIO();
	}
	connection->cleanPollFds();
}
