#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include "Request.hpp"

typedef struct {
	std::string	body;
	int			code;
}	htmlResponse;

class CGIServer {
	public:
		CGIServer(const std::string scriptPath);

		void			setEnv(RequestInfo &requestInfo);
		htmlResponse	executeScript(std::string requestData);

	private:
		htmlResponse						GBIReturn;
		const std::string					scriptPath;
		std::map<std::string, std::string>	envVars;

		void	getEnvp(char *envp[]);
		void	readChildReturn(int pipefd[], int pipefderror[]);
};

#endif
