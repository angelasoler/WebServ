
#include "EventLoop.hpp"
#include "Config.hpp"
#include <csignal>
#define PORT 8080

int main() {
	Config* config = Config::getInstance();

	config->loadConfig("config.conf");
	N_SERVERS = config->servers.size();

	std::vector<Server>	servers;
	EventLoop			loop;

	for (uint i = 0; i < N_SERVERS; i++)
	{
		struct pollfd	server_poll_fd;
		Server			tmp(config->servers[i]);

		servers.push_back(tmp);
		server_poll_fd.fd = servers[i].fd;
		server_poll_fd.events = POLLIN;
		loop.connection->poll_fds.push_back(server_poll_fd);
	}

	// printConfig(config);
	loop.run(servers);
	return 0;
}
