
#include "Request.hpp"

Request::Request(void) {}

Request::~Request(void) {}

int	Request::readRequest(int client_fd, std::map<int, std::string> &request)
{
	char	buffer[BUFFER_SIZE];

	memset(buffer, 0, BUFFER_SIZE);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = '\0';
	if (bytes_read < 0)
	{
		std::cerr << "Read failed" << std::endl;
		// handleError("Read failed");
	}
	else if (bytes_read == 0)
	{
		std::cout << "Conexão fechada pelo cliente" << std::endl;
		close(client_fd);
		return (1);
	}
	else
	{
		request[client_fd] += buffer;
		std::cout // TO-DO: adicionar diretiva DEBUG
		// << buffer
		<< std::endl;
	}
	return (0);
}

void	breakIntoLines(std::vector<std::string> &lines, std::string text) {
	char	*tokenLine;

	tokenLine = std::strtok((char *)text.c_str(), "\n");
	do {
		std::string	tmp(tokenLine);
		lines.push_back(tmp);
		tokenLine = std::strtok(NULL, "\n");
	} while (tokenLine);
}

std::map< std::string, std::vector<std::string> > &Request::getHeader(void)
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

void	Request::dataStrcuture(std::string text)
{
	std::vector<std::string>	lines;

	breakIntoLines(lines, text);
	breakResquesLine(lines[0]);
	parseTheOthers(lines);
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


e_httpMethodActions	Request::parseRequest(std::string text)
{
	dataStrcuture(text); //leak
	// printHeaderDataStructure();
 	if (header["request"][METHOD] == "GET")
	{
		std::cout << "\tRESPONSE\n" << std::endl;
		return(RESPONSE);
	}
	else if (header["request"][METHOD] == "POST")
	{
		std::cout << "\tPOST\n" << std::endl;
		return(UPLOAD);
	}
	else if (header["request"][METHOD] == "DELETE")
	{
		std::cout << "\tDELETE\n" << std::endl;
		return(DELETE);
	}
	else
	{
		std::cout << "\tCLOSE\n" << std::endl;
		return(CLOSE);
	}
}
