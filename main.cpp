
#include "EventLoop.hpp"
#include <csignal>
#define PORT 8080

// TO-DO
// [_] Clean staff
// [_] Delegate to class SigHandler
void	killing(int sig)
{
	(void)sig;
	exit(5);
}

int main() {
	Server		server(PORT);
	EventLoop	loop(server);

	signal(SIGINT, killing);
	loop.run();
	return 0;
}
