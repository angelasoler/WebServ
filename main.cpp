
#include "server.hpp"
#include <csignal>

Server server(PORT);

void    killing(int sig)
{
	(void)sig;
	exit(5);
}

int main() {
	signal(SIGINT, killing);
	server.run();
	return 0;
}
