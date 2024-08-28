#include "CGIServer.hpp"
#include <cerrno>
#include <sstream>

CGIServer::CGIServer(const std::string scriptPath) : scriptPath(scriptPath) {}

// TO-DO: verificar o script path do routes?
void	CGIServer::setEnv(RequestInfo &requestInfo)
{
	std::ostringstream oss;

	oss << requestInfo.body.size();
	if (requestInfo.action == RESPONSE)
		envVars["REQUEST_METHOD"] = "GET";
	if (requestInfo.action == UPLOAD)
		envVars["REQUEST_METHOD"] = "POST";
	envVars["SCRIPT_NAME"] = scriptPath;
	envVars["SERVER_PROTOCOL"] = "HTTP/1.1";
	envVars["CONTENT_LENGTH"] = oss.str();
}

// TO-DO: verifica se está corretamente alocado
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

void redirChildPipes(int pipefd[], int pipefderror[])
{
	dup2(pipefd[0], STDIN_FILENO);
	close(pipefd[0]);
	dup2(pipefd[1], STDOUT_FILENO);
	close(pipefd[1]);
	close(pipefderror[0]);
	dup2(pipefderror[1], STDERR_FILENO);
	close(pipefderror[1]);
}

void CGIServer::readChildReturn(int pipefd[], int pipefderror[])
{
	char buffer[4096];
	char buffererror[4096];
	ssize_t count;
	std::string buffererror2;
	std::string buffer2;

	count = read(pipefd[0], buffer, sizeof(buffer) - 1);
	close(pipefd[0]);
	buffer[count] = '\0';
	buffer2 = buffer;
	if (!buffer2.empty()) {
		std::cout << "\t\t======CGI stdout=======" << std::endl;
		close(pipefderror[0]);
		std::cout << buffer;
		GBIReturn.body = buffer;
		GBIReturn.code = 200;
	}
	else {
		count = read(pipefderror[0], buffererror, sizeof(buffererror) - 1);
		close(pipefderror[0]);
		buffererror[count] = '\0';
		buffererror2 = buffererror;
	}
	if (!buffererror2.empty()) {
		std::cout << "\t\t======CGI stderr=======" << std::endl;
		std::cout << buffererror;
		GBIReturn.body = buffererror;
		GBIReturn.code = 400;
	}
}

// TO-DO: pegar stderror só para debug, retornar menssagem padrão
// pegar codigo de retorno do script?
htmlResponse	CGIServer::executeScript(std::string requestData) {
	std::cout << "\t\t======CGI exec=======" << std::endl;
	int	pipefd[2];
	int	pipefderror[2];

	if (pipe(pipefd) == -1 || pipe(pipefderror) == -1)
		throw(std::runtime_error("Pipe creation failed."));
	pid_t pid = fork();
	if (pid < 0)
		throw(std::runtime_error("Fork failed."));
	else if (pid == 0) {
		char *envp[envVars.size() + 1];

		redirChildPipes(pipefd, pipefderror);
		getEnvp(envp);
		std::string pyBin = "/usr/bin/python3";
		//TO-DO: fazer cd para stacticScriptPath
		char* const argv[] = {const_cast<char*>(pyBin.c_str()), const_cast<char*>(scriptPath.c_str()), NULL};
		execve(argv[0], argv, envp);
	} else {
		if (!requestData.empty())
			std::cout << "requestData: " << requestData << std::endl;
		write(pipefd[1], requestData.c_str(), requestData.size());
		close(pipefd[1]);
		close(pipefderror[1]);
		waitpid(pid, NULL, 0);
		readChildReturn(pipefd, pipefderror);
		return (GBIReturn);
	}
	return GBIReturn;
}
