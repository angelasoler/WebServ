
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
		std::cout
		// << "Request: "
		// << buffer
		<< std::endl;
	}
	return (0);
}

e_httpMethodActions	Request::parseRequest(std::string text)
{
	if (!std::strncmp(text.c_str(), "GET", 3)
		|| !std::strncmp(text.c_str(), "POST", 4)
		|| !std::strncmp(text.c_str(), "DELETE", 6))
	{
		std::cout << "RESPONSE\n" << std::endl;
		return(RESPONSE);
	}
	else
	{
		std::cout << "CLOSE\n" << std::endl;
		return(CLOSE);
	}
}
