
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

std::vector<std::string> insertAllowedMethods(void)
{
	std::vector<std::string>	vec;

	vec.push_back("GET");
	vec.push_back("POST");
	vec.push_back("DELETE");
	return vec;
}

int	Request::getInformationalStatus(void)
{
	std::vector<std::string> 			allowedMethods = insertAllowedMethods();

	std::vector<std::string>::iterator	requestedMethod;


	if (!requestReader.getHttpVersion().empty() && requestReader.getHttpVersion() != "HTTP/1.1")
		return 400;
	requestedMethod = std::find (allowedMethods.begin(), allowedMethods.end(), requestReader.getMethod());
	if (requestedMethod != allowedMethods.end())
	{
		requestedMethod = std::find(info.configRef.accepted_methods.begin(),
										info.configRef.accepted_methods.end(),
										requestReader.getMethod());
		if (requestedMethod != info.configRef.accepted_methods.end())
		{
			return 200;
		}
		return 405; // Not ALLOWED
	}
	return 400; // Bad
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.rawBody = requestReader.getRawBody();
	info.body = requestReader.getBody();
	info.contentType = requestReader.getHeader("Content-Type");
	info.requestedRoute = requestReader.getRequestedRoute();
	adjustRoute(info.requestedRoute);
	info.serverRef = serverConfig;
}

void Request::parseRequest(ServerConfig &serverConfig)
{
	parseRequestInfo(serverConfig);
	ParsePathInfo::parsePathInfo(info);
	int	status = getInformationalStatus();
	if (status != 200)
	{
		info.action = RESPONSE;
		info.pathType = UNKNOWN;
		if (status == 405)
			info.requestedRoute = "";
		else
			info.requestedRoute = "Bad";
		return ;
	}
	ParseBodyInfo::parseBodyInfo(info);
	PrintRequestInfo::printRequestInfo(info);
}

e_httpMethodActions	Request::getMethodAction(void)
{
	// printHeaderDataStructure();
	std::vector<std::string> allowedMethods = insertAllowedMethods();

	std::vector<std::string>::iterator	requestedMethod;

	requestedMethod = std::find (allowedMethods.begin(), allowedMethods.end(), requestReader.getMethod());
	if (requestedMethod != allowedMethods.end())
	{
		std::cout << "\t" << *requestedMethod << "\n";
		return ((e_httpMethodActions)(requestedMethod - allowedMethods.begin()));
	}
	if (!requestReader.getMethod().empty())
		return (RESPONSE);
	std::cout << "\tCLOSE\n" << std::endl;
	return(CLOSE);
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