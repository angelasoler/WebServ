#include "EventLoop.hpp"
#include "Config.hpp"

void endConfig(void)
{
	Config::getInstance()->freeInstance();
}

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
	printConfig();
	return (1);
}

int main(int argc, char* argv[])
{
	if (!initializeConfig(argc, argv)) {
		return (1);
	}
	EventLoop	eventLoop;

	eventLoop.run();
	endConfig();
	return 0;
}
