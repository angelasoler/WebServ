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
		CGIServer(RequestInfo &info);

		void			setEnv(void);
		htmlResponse	executeScript(void);

	private:
		RequestInfo							&requestInfo;
		htmlResponse						GBIReturn;
		std::string							scriptPath;
		std::map<std::string, std::string>	envVars;
		int									pipefd[2];
		int									pipefderror[2];


		void	getEnvp(char *envp[]);
		void	readChildReturn(void);
		void	redirChildPipes(void);
};

#endif
