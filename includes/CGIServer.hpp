#ifndef CGI_HPP
#define CGI_HPP

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <map>
#include <ctime>
#include "Request.hpp"

typedef struct {
	std::string	body;
	int			code;
}	htmlResponse;

class CGIServer {
	public:
		htmlResponse	CGIReturn;

		CGIServer(RequestInfo &info);

		void	setEnv(void);
		void	executeScript(void);

	private:
		RequestInfo							&requestInfo;
		std::string							scriptPath;
		std::string							execDir;
		std::map<std::string, std::string>	envVars;
		int									pipefd[2];
		int									pipefderror[2];

		std::clock_t	start;

		void	getEnvp(char *envp[]);
		void	readChildReturn(void);
		void	redirChildPipes(void);
		void	CGIFeedLog(std::string buffer);
		void	waitAndReadChild(pid_t pid);
		bool	verifyRequestedScript(void);
};

#endif