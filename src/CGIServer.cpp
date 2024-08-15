#include "CGIServer.hpp"
#include <cerrno>
#include <sstream>

CGIServer::CGIServer(const std::string scriptPath) : scriptPath(scriptPath) {}

void	CGIServer::setEnv(RequestInfo &requestInfo)
{
	if (requestInfo.action == RESPONSE)
		envVars["REQUEST_METHOD"] = "GET";
	if (requestInfo.action == UPLOAD)
		envVars["REQUEST_METHOD"] = "POST";
	envVars["SCRIPT_NAME"] = scriptPath;
	envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
	// envVars["QUERY_STRING"] = "";
	envVars["CONTENT_TYPE"] = "multipart/form-data; boundary=--------------------------d25a4bd4b4883f46\r\n";
	envVars["CONTENT_LENGTH"] = "0";
	envVars["HTTP_HOST"] = "localhost";
	envVars["SERVER_NAME"] = "localhost";
	envVars["SERVER_PORT"] = "8080";
	envVars["REMOTE_ADDR"] = "127.0.0.1";
	// envVars["REMOTE_PORT"] = "12345";
}

std::string	CGIServer::executeScript(std::string requestData) {
	std::cout << "\t\t======CGI exec=======" << std::endl;
	std::ostringstream oss;
	oss << requestData.size();
	envVars["CONTENT_LENGTH"] = oss.str();
	int	pipefd[2];

	if (pipe(pipefd) == -1)
		throw(std::runtime_error("Pipe creation failed."));

	pid_t pid = fork();
	if (pid < 0)
		throw(std::runtime_error("Fork failed."));
	else if (pid == 0) {
		dup2(pipefd[0], STDIN_FILENO);
		close(pipefd[0]);
		dup2(pipefd[1], STDOUT_FILENO);
		close(pipefd[1]);

		char* envp[envVars.size() + 1];
		int i = 0;
		for (std::map<std::string, std::string>::const_iterator it = envVars.begin(); it != envVars.end(); ++it) {
			std::string envEntry = it->first + "=" + it->second;
			envp[i] = new char[envEntry.size() + 1];
			std::strcpy(envp[i], envEntry.c_str());
			++i;
		}
		envp[i] = NULL;
		std::string pyBin = "/usr/bin/python3";
		std::string stacticScriptPath = std::string(getcwd(NULL, 0)) + "/cgi-bin" + scriptPath;

		char* const argv[] = {const_cast<char*>(pyBin.c_str()), const_cast<char*>(stacticScriptPath.c_str()), NULL};
		if (execve(argv[0], argv, envp))
			throw(std::runtime_error(std::strerror(errno)));
	} else {
		std::cout << "requestData: " << requestData << std::endl;
		write(pipefd[1], requestData.c_str(), requestData.size());
		close(pipefd[1]);
		waitpid(pid, NULL, 0);

		char buffer[4096];
		ssize_t count;
		count = read(pipefd[0], buffer, sizeof(buffer) - 1);
		close(pipefd[0]);
		buffer[count] = '\0';
		std::cout << "\t\t======CGI stdout=======" << std::endl;
		std::cout << buffer;
		return (buffer);
	}
	return NULL;
}
