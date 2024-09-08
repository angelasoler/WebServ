#include "CGIServer.hpp"
#include <cerrno>
#include <sstream>
#include "TimeNow.hpp"

CGIServer::CGIServer(RequestInfo &info) : requestInfo(info), scriptPath(info.fullPath) {}

void	CGIServer::setEnv(void)
{
	std::ostringstream oss;
	size_t pos = scriptPath.rfind("/");

	execDir = scriptPath.substr(0, pos);
	scriptPath = scriptPath.substr(pos+1);
	if (requestInfo.action == RESPONSE)
		envVars["REQUEST_METHOD"] = "GET";
	else
		envVars["REQUEST_METHOD"] = "POST";
	envVars["CONTENT_TYPE"] = requestInfo.contentType;
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
		envp[i] = (char *)std::calloc(envEntry.size() + 1, sizeof(char));
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
		logFd.close();
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
	logFd.close();
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
	int exit_code = 0;

	if (!waitpid(pid, &child_exit_status, WNOHANG)) {
		while (double(std::clock() - start) / CLOCKS_PER_SEC <= 2.0) {
			if (waitpid(pid, &child_exit_status, WNOHANG))
				break ;
		}
		if (!waitpid(pid, &child_exit_status, WNOHANG)) {
			kill(pid, SIGKILL);
			waitpid(pid, &child_exit_status, 0);
			child_exit_status = 1;
		}
	}
	readChildReturn();
	if (WIFEXITED(child_exit_status))
		exit_code = WEXITSTATUS(child_exit_status);
	if (exit_code) {
		CGIReturn.code = 500;
		CGIReturn.body.clear();
	}
}

bool	CGIServer::verifyRequestedScript(void)
{
	if (!requestInfo.permissions.execute)
		CGIReturn.code = 405;
	else if (requestInfo.permissions.notFound)
		CGIReturn.code = 404;
	else
		return false;
	CGIReturn.body.clear();
	return true;
}

void	CGIServer::executeScript(void)
{
	if (verifyRequestedScript())
		return ;
	char		*envp[envVars.size() + 1];
	char		*rawPtr = requestInfo.rawBody.data();
	std::string	pyBin = "/usr/bin/python3";
	char		*const argv[] = {const_cast<char*>(pyBin.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};

	start = std::clock();
	getEnvp(envp);
	if (pipe(pipefd) == -1 || pipe(pipefderror) == -1)
		std::cerr << "Pipe creation failed." << std::endl;
	pid_t pid = fork();
	if (pid < 0)
		std::cerr << "Fork failed." << std::endl;
	else if (pid == 0) {
		chdir(execDir.c_str());
		redirChildPipes();
		execve(argv[0], argv, envp);
	} else {
		write(pipefd[1], rawPtr, requestInfo.rawBody.size());
		close(pipefd[1]);
		close(pipefderror[1]);

		waitAndReadChild(pid);
	}
	for (size_t i = 0; i < envVars.size() + 1; i++)
		std::free(envp[i]);
	return ;
}
