
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


bool Request::findEndRequest(const std::vector<char> &vec) {
	ssize_t size = vec.size();

	if (size < 4) {
		return false;
	}
	char last1 = vec[size - 4];
	char last2 = vec[size - 3];
	char last3 = vec[size - 2];
	char last4 = vec[size - 1];

	return (last1 == '\r' && last2 == '\n' && last3 == '\r' && last4 == '\n');
}

ssize_t Request::findContentLength(const std::vector<char>& clientRequestText) {
	const char* contentLengthStr = "Content-Length: ";
	std::vector<char>::const_iterator it = std::search(clientRequestText.begin(), clientRequestText.end(),
													   contentLengthStr, contentLengthStr + 16);

	if (it != clientRequestText.end()) {
		ssize_t start_pos = std::distance(clientRequestText.begin(), it) + 16;
		it = std::find(clientRequestText.begin() + start_pos, clientRequestText.end(), '\r');
		if (it != clientRequestText.end()) {
			ssize_t end_pos = std::distance(clientRequestText.begin(), it);
			std::string content_length_str(clientRequestText.begin() + start_pos, clientRequestText.begin() + end_pos);

			ssize_t content_length = std::atoi(content_length_str.c_str());
			return content_length;
		}
	}
	return -1;
}

bool Request::isComplete(const std::vector<char>& clientRequestText) {

	if (header_end == -1) {
		const char* headerEnd = "\r\n\r\n";
		std::vector<char>::const_iterator it = std::search(clientRequestText.begin(), clientRequestText.end(),
														   headerEnd, headerEnd + 4);
		if (it == clientRequestText.end()) {
			return true;
		}
		header_end = std::distance(clientRequestText.begin(), it);
	}
	if (content_length == -1) {
		content_length = findContentLength(clientRequestText);
	}

	if (content_length != -1)
	{
		ssize_t total_length = header_end + 4 + content_length;
		if ((ssize_t)clientRequestText.size() >= total_length) {
			return true;
		}
		else {
			return false;
		}
	}


	if (findEndRequest(clientRequestText)) {
		return true;
	}

	return false;
}

bool	Request::readRequest(int client_fd)
{
	if (info.action != AWAIT_READ) {
		header_end = -1;
		content_length = -1;
		buffer_limit = 20971520;
		requestReader = RequestReader();
	}

	std::vector<char> buffer(buffer_limit);
	ssize_t bytes_read;

	while (true) {
		
		bytes_read = recv(client_fd, &buffer[0], buffer_limit, 0);

		if (bytes_read == 0) {
			info.action = CLOSE;
			break ;
		}

		if (bytes_read < 0) {
			info.action = AWAIT_READ;
			break ;
		}
		requestVec.insert(requestVec.end(), buffer.begin(), buffer.begin() + bytes_read);
		if (isComplete(requestVec)) {
			requestReader.readHttpRequest(requestVec);
			info.action = RESPONSE;
			break;
		}
	}
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
		requestedMethod = std::find(info.routeRef.accepted_methods.begin(),
										info.routeRef.accepted_methods.end(),
										requestReader.getMethod());
		if (requestedMethod != info.routeRef.accepted_methods.end())
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
	routeRef(),
	contentType(""),
	action(RESPONSE),
	body(""),
	multipartHeaders(),
	multipartValues(),
	urlencodedValues()
	{}