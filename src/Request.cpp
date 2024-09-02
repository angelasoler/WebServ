
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
	requestsText.clear();
	info = RequestInfo();
	requestReader = RequestReader();

	if (!requestReader.readHttpRequest(client_fd))
		return (true);
	requestsText = requestReader.getFullRequest();
	printRequest();
	return false;
}

void	adjustRoute(std::string &route)
{
	std::string newRoute;
	bool		appendSlash = false;
	
	newRoute.reserve(route.size());
	for (std::string::iterator it = route.begin(); it != route.end(); it++)
	{
		if (*it == '/')
		{
			if (!appendSlash)
			{
				newRoute += *it;
				appendSlash = true;
			}
		}
		else
		{
			newRoute += *it;
			appendSlash = false;
		}
	}
	route = newRoute;
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.body = requestReader.getBody();
	info.contentType = requestReader.getHeader("Content-Type");

	if (info.contentType.find("multipart/form-data") != std::string::npos) {
		info.multipartHeaders = requestReader.getMultipartHeaders();
		info.multipartValues = requestReader.getMultipartValues();
	}
	info.requestedRoute = requestReader.getRequestedRoute();
	adjustRoute(info.requestedRoute);
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

RequestInfo::RequestInfo() :
	requestedRoute("\\"),
	fullPath(""),
	pathType(UNKNOWN),
	permissions(),
	serverRef(),
	configRef(),
	contentType(""),
	action(RESPONSE),
	body(""),
	multipartHeaders(),
	multipartValues(),
	urlencodedValues()
	{}