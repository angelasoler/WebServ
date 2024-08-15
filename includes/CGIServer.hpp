#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include "Request.hpp"

class CGIServer {
	public:
		CGIServer(const std::string scriptPath);

		void		setEnv(RequestInfo &requestInfo);
		std::string	executeScript(std::string requestData);

	private:
		const std::string					scriptPath;
		std::map<std::string, std::string>	envVars;
};
