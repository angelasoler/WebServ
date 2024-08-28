
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include "ParseBodyInfo.hpp"
#include "PrintRequestInfo.hpp"
#include "TimeNow.hpp"
#include <fstream>
#include <cerrno>

Request::Request(void) {}

Request::~Request(void) {}

void	Request::printRequest(void)
{
	std::ofstream	requestLog("logs/request.log", std::ios_base::app);

	requestLog << TimeNow()
				<< std::endl;
	requestLog << requestsText
				<< std::endl;
	requestLog.close();
}

bool Request::readRequest(int client_fd)
{
	char buffer[BUFFER_SIZE + 1];

	requestCompleted = false;
	memset(buffer, 0, BUFFER_SIZE + 1);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);

	std::cout << "Chamada read request\n";
	if (bytes_read <= 0 || bytes_read == 0) {
		// requestCompleted = true;
		return true;
	}

	// Adicionar os dados lidos ao vetor de requests
	requestsVector.insert(requestsVector.end(), buffer, buffer + bytes_read);

	// Verificar se os headers completos foram recebidos
	std::vector<char>::iterator it = std::search(requestsVector.begin(), requestsVector.end(),
												 "\r\n\r\n", "\r\n\r\n" + 4);

	if (it != requestsVector.end()) {
		std::size_t headerEnd = std::distance(requestsVector.begin(), it);

		// Procurar pelo "Content-Length: "
		std::vector<char>::iterator itContentLength = std::search(requestsVector.begin(), requestsVector.end(),
																  "Content-Length: ", "Content-Length: " + 16);

		if (itContentLength != requestsVector.end()) {
			std::size_t contentStart = headerEnd + 4;
			int contentLength = std::atoi(std::string(itContentLength + 16, requestsVector.end()).c_str());

			// Verificar se o vetor contém o corpo completo
			if (requestsVector.size() >= contentStart + contentLength) {
				// Requisição completa (headers + corpo)
				requestCompleted = true;
				std::cout << "Headers recebidos, com Content-Length\n";
				requestsText.append(requestsVector.begin(), requestsVector.end());
				printRequest();
				requestsVector.clear();
				return false;
			}
		} else {
			// Não há Content-Length, então os headers são suficientes
			requestCompleted = true;
			std::cout << "Headers recebidos, sem Content-Length\n";
			requestsText.append(requestsVector.begin(), requestsVector.end());
			printRequest();
			requestsVector.clear();
			return false;
		}
	}

	// A requisição ainda não está completa
	return false;
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
	printHeaderDataStructure();
}

void	Request::printHeaderDataStructure(void)
{
	std::map< std::string, std::vector<std::string> >::iterator	headerIt;
	std::ofstream		parsedRequest("logs/parsedHeader.log");

	parsedRequest << "\t\t === parsed header ==="  << std::endl;
	for (headerIt = header.begin(); headerIt != header.end(); headerIt++) {
		parsedRequest << headerIt->first << std::endl;
		std::vector<std::string>::iterator it;
		for (it = headerIt->second.begin(); it != headerIt->second.end(); it++)
			parsedRequest << "\t" << *it << std::endl;
	}
	parsedRequest << "\t\t === \t\t ==="  << std::endl;
	parsedRequest.close();
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.requestedRoute = header["request"][ROUTE];
	if (header.find("Content-Type") != header.end() && !header["Content-Type"].empty())
	{
		info.contentType = header["Content-Type"][0];
	}
	info.serverRef = serverConfig;
}

void Request::parseRequest(ServerConfig &serverConfig)
{

	static int ref;

	ref++;
	// std::cout << requestsText << " " << ref << "\n";
	// std::cout << response.getClientFd() << "\n";
	parseRequestHeader();
	parseRequestInfo(serverConfig);
	ParsePathInfo::parsePathInfo(info);
	ParseBodyInfo::parseBodyInfo(requestsText, info);
	PrintRequestInfo::printRequestInfo(info);
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
