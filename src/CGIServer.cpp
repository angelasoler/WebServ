#include "CGIServer.hpp"
#include <cerrno>
#include <sstream>
#include "TimeNow.hpp"

CGIServer::CGIServer(RequestInfo &info) : requestInfo(info), scriptPath(info.fullPath) {}

void	CGIServer::setEnv(void)
{
	std::ostringstream oss;
	// std::string tmp = scriptPath;
	// size_t pos = tmp.rfind("/");

	// execDir = tmp.substr(0, pos);
	oss << requestInfo.body.size();
	if (requestInfo.action == RESPONSE)
		envVars["REQUEST_METHOD"] = "GET";
	else
		envVars["REQUEST_METHOD"] = "POST";
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
	std::ofstream	logFd("logs/CGI.log", std::ios_base::app);
	std::string	buffererror2;
	char		buffererror[4096];
	ssize_t		count;
	std::string CGIbody(requestInfo.rawBody.begin(), requestInfo.rawBody.end());

	logFd << "\n" << TimeNow();
	if (!CGIbody.empty())
		logFd << "requestInfo.rawBody: " << CGIbody << std::endl;
	if (!buffer.empty()) {
		logFd << "\t\t======CGI stdout=======" << std::endl;
		close(pipefderror[0]);
		logFd << buffer;
		return ;
	}
	count = read(pipefderror[0], buffererror, sizeof(buffererror) - 1);
	close(pipefderror[0]);
	buffererror[count] = '\0';
	buffererror2 = buffererror;
	if (!buffererror2.empty()) {
		logFd << "\t\t======CGI stderr=======" << std::endl;
		logFd << buffererror;
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
		if (requestInfo.action == RESPONSE)
			CGIReturn.code = 200;
		else
			CGIReturn.code = 201;
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
	if (requestInfo.permissions.notFound) {
		CGIReturn.code = 404;
		CGIReturn.body.clear();
		return ;
	}


	if (pipe(pipefd) == -1 || pipe(pipefderror) == -1)
		std::cerr << "Pipe creation failed." << std::endl;
	pid_t pid = fork();
	if (pid < 0)
		std::cerr << "Fork failed." << std::endl;
	else if (pid == 0) {
		char *envp[envVars.size() + 1];
		std::string pyBin = "/usr/bin/python3";
		char* const argv[] = {const_cast<char*>(pyBin.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};

		// chdir(execDir.c_str());
		redirChildPipes();
		getEnvp(envp);
		execve(argv[0], argv, envp);
	} else {
		char* rawPtr = requestInfo.rawBody.data();
		write(pipefd[1], rawPtr, requestInfo.rawBody.size());
		close(pipefd[1]);
		close(pipefderror[1]);

		waitAndReadChild(pid);
	}
	return ;
}
