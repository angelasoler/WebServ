
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include "TimeNow.hpp"
#include <fstream>

Request::Request(void) {}

Request::~Request(void) {}

void	Request::printRequest(void)
{
	std::ofstream	requestLog("logs/request.log", std::ios_base::app);

	requestLog << TimeNow()
				<< std::endl;
	requestLog << requestsText
				<< std::endl;
}

void	Request::readRequest(int client_fd)
{
	char	buffer[BUFFER_SIZE + 1];

	memset(buffer, 0, BUFFER_SIZE + 1);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = '\0';
	if (bytes_read < 0) {
		close(client_fd); //limpar poll_fd
		return ;
	}
	else if (bytes_read == 0)
	{
		std::cout << "Conexão fechada pelo cliente" << std::endl;
		close(client_fd); //limpar poll_fd
		return ;
	}
	else
	{
		requestsText += buffer;
		// printRequest();
	}
	return ;
}

void	Request::breakIntoLines(std::vector<std::string> &lines) {
	char	*tokenLine;

	tokenLine = std::strtok((char *)requestsText.c_str(), "\n");
	do {
		std::string	tmp(tokenLine);
		lines.push_back(tmp);
		tokenLine = std::strtok(NULL, "\n");
	} while (tokenLine);
}

std::map< std::string, std::vector<std::string> >	&Request::getHeader(void)
{
	return (header);
}

void	Request::cleanHeader(void)
{
	std::map< std::string, std::vector<std::string> >::iterator	headerIt;

	for (headerIt = header.begin(); headerIt != header.end(); headerIt++)
		headerIt->second.clear();
}

void	Request::breakResquesLine(std::string &line) {
	std::istringstream ss(line);

	while (ss) {
		std::string token;
		ss >> token;
		header["request"].push_back(token);
	}
}

void	Request::parseTheOthers(std::vector<std::string> &lines)
{
	std::vector<std::string>::iterator	it1;

	for (it1 = (lines.begin() + 1); it1 != lines.end(); it1++) {
		std::string str = *it1;
		size_t pos = str.find(":");
		if (pos == std::string::npos)
			break ;
		std::string tokenKey = str.substr(0, pos);
		std::string tokenValue = str.substr((pos + 1), str.size());
		header[tokenKey].push_back(tokenValue); 
	}
}

void	Request::parseRequestHeader(void)
{
	std::vector<std::string>	lines;

	cleanHeader();
	breakIntoLines(lines);
	breakResquesLine(lines[0]);
	parseTheOthers(lines);
	// printHeaderDataStructure();
}

void	Request::printHeaderDataStructure(void)
{
	std::map< std::string, std::vector<std::string> >::iterator	headerIt;

	std::cout << "\t\t === parse header ==="  << std::endl;
	for (headerIt = header.begin(); headerIt != header.end(); headerIt++) {
		std::cout << headerIt->first << std::endl;
		std::vector<std::string>::iterator it;
		for (it = headerIt->second.begin(); it != headerIt->second.end(); it++)
			std::cout << "\t" << *it << std::endl;
	}
	std::cout << "\t\t === \t\t ==="  << std::endl;
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.requestedRoute = header["request"][ROUTE];
	info.serverRef = serverConfig;
}

void Request::parseRequest(ServerConfig &serverConfig)
{
	parseRequestHeader();
	parseRequestInfo(serverConfig);
	ParsePathInfo::parsePathInfo(info);
}

e_httpMethodActions	Request::getMethodAction(void)
{
	// printHeaderDataStructure();
	if (header["request"][METHOD] == "GET")
	{
		std::cout << "\tRESPONSE\n" << std::endl;
		return (RESPONSE);
	}
	else if (header["request"][METHOD] == "POST")
	{
		std::cout << "\tPOST\n" << std::endl;
		return (UPLOAD);
	}
	else if (header["request"][METHOD] == "DELETE")
	{
		std::cout << "\tDELETE\n" << std::endl;
		return (DELETE);
	}
	else
	{
		std::cout << "\tCLOSE\n" << std::endl;
		return(CLOSE);
	}
}
