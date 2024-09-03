#include "CGIServer.hpp"
#include <cerrno>
#include <sstream>

CGIServer::CGIServer(RequestInfo &info) : requestInfo(info), scriptPath(info.fullPath) {}

void	CGIServer::setEnv(void)
{
	std::ostringstream oss;

	oss << requestInfo.body.size();
	switch (requestInfo.action)
	{
		case RESPONSE:
			envVars["REQUEST_METHOD"] = "GET";
			break;
		case UPLOAD:
			envVars["REQUEST_METHOD"] = "POST";
			break;
		default:
			return;
	}
	envVars["SCRIPT_NAME"] = scriptPath;
	envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
	envVars["CONTENT_LENGTH"] = oss.str();
	envVars["QUERY_STRING"] = requestInfo.queryString;
}

void CGIServer::getEnvp(char *envp[])
{
	int i = 0;
	for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
		std::string envEntry = it->first + "=" + it->second;
		envp[i] = new char[envEntry.size() + 1];
		std::strcpy(envp[i], envEntry.c_str());
		++i;
	}
	envp[i] = NULL;
}

void CGIServer::redirChildPipes(void)
{
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	close(pipefderror[0]);
	dup2(pipefderror[1], STDERR_FILENO);
	close(pipefderror[1]);
}

void	CGIServer::CGIFeedLog(std::string buffer)
{
	std::string	buffererror2;
	char		buffererror[4096];
	ssize_t		count;

	if (!requestInfo.body.empty())
		std::cout << "requestInfo.body: " << requestInfo.body << std::endl;
	if (!buffer.empty()) {
		std::cout << "\t\t======CGI stdout=======" << std::endl;
		close(pipefderror[0]);
		std::cout << buffer;
		return ;
	}
	count = read(pipefderror[0], buffererror, sizeof(buffererror) - 1);
	close(pipefderror[0]);
	buffererror[count] = '\0';
	buffererror2 = buffererror;
	if (!buffererror2.empty()) {
		std::cout << "\t\t======CGI stderr=======" << std::endl;
		std::cout << buffererror;
	}
}

void CGIServer::readChildReturn(void)
{
	char buffer[4096];
	ssize_t count;
	
	std::string buffer2;

	count = read(pipefd[0], buffer, sizeof(buffer) - 1);
	close(pipefd[0]);
	buffer[count] = '\0';
	buffer2 = buffer;
	if (!buffer2.empty()) {
		CGIReturn.body = buffer;
		CGIReturn.code = 200; //201 se for post
	}
	CGIFeedLog(buffer2);
}

void	CGIServer::waitAndReadChild(pid_t pid)
{
	int	child_exit_status;
	int exit_code;

	waitpid(pid, &child_exit_status, 0);
	readChildReturn();
	if (WIFEXITED(child_exit_status))
		exit_code = WEXITSTATUS(child_exit_status);
	if (exit_code) {
		CGIReturn.code = 500;
		CGIReturn.body.clear();
	}
}

void	CGIServer::executeScript(void)
{
	if (!requestInfo.permissions.execute) {
		CGIReturn.code = 405;
		CGIReturn.body.clear();
		return ;
	}
	std::cout << "\t\t======CGI exec=======" << std::endl;

	if (pipe(pipefd) == -1 || pipe(pipefderror) == -1)
		throw(std::runtime_error("Pipe creation failed.")); //mandar no log
	pid_t pid = fork();
	if (pid < 0)
		throw(std::runtime_error("Fork failed.")); //mandar no log
	else if (pid == 0) {
		char *envp[envVars.size() + 1];

		redirChildPipes();
		getEnvp(envp);
		std::string pyBin = "/usr/bin/python3";
		//TO-DO: fazer cd para stacticScriptPath
		char* const argv[] = {const_cast<char*>(pyBin.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		execve(argv[0], argv, envp);
	} else {
		write(pipefd[1], requestInfo.body.c_str(), requestInfo.body.size());
		close(pipefd[1]);
		close(pipefderror[1]);

		waitAndReadChild(pid);
	}
	return ;
}
