#include "EventLoop.hpp"
#include "Config.hpp"


int initializeConfig(int argc, char* argv[]) {
	Config	*config = Config::getInstance();

	if (argc == 1) {
		config->loadDefaultConfig();
	} else if (argc == 2) {
		config->loadConfig(argv[1]);
	} else {
		std::cerr << "Usage: ./webserv [config_file.conf]" << std::endl;
		return (0);
	}
	// printConfig(config);
	return (1);
}

int main(int argc, char* argv[])
{
	if (!initializeConfig(argc, argv)) {
		return (1);
	}
	EventLoop	eventLoop;

	eventLoop.run();
	return 0;
}
