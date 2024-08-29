
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include "ParseBodyInfo.hpp"
#include "RequestReader.hpp"
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

bool	Request::readRequest(int client_fd)
{
	requestReader = RequestReader();

	if (!requestReader.readHttpRequest(client_fd))
		return (true);
	requestsText = requestReader.getRequest();
	printRequest();
	return false;
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.body = requestReader.getBody();

	// Extract Content Type and Boundary
	std::string	 contentTypeLine = requestReader.getHeader("Content-Type");
	if (!contentTypeLine.empty()) {
		size_t pos = contentTypeLine.find("boundary=", 0);
		if (pos != std::string::npos)
			info.boundary = contentTypeLine.substr(pos + 9);
		info.contentType = contentTypeLine;
    }

	info.requestedRoute = requestReader.getRequestedRoute();
	info.serverRef = serverConfig;
}

void Request::parseRequest(ServerConfig &serverConfig)
{
	parseRequestInfo(serverConfig);
	ParsePathInfo::parsePathInfo(info);
	ParseBodyInfo::parseBodyInfo(info);
	PrintRequestInfo::printRequestInfo(info);
}

e_httpMethodActions	Request::getMethodAction(void)
{
	// printHeaderDataStructure();
	if (requestReader.getMethod() == "GET")
	{
		std::cout << "\tRESPONSE\n" << std::endl;
		return (RESPONSE);
	}
	else if (requestReader.getMethod() == "POST")
	{
		std::cout << "\tPOST\n" << std::endl;
		return (UPLOAD);
	}
	else if (requestReader.getMethod() == "DELETE")
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
