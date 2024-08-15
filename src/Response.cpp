#include "Response.hpp"
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include <sys/socket.h>
#include <unistd.h>

// CONSTRUCTOR AND DESTRUCTOR

Response::Response(RequestInfo info, int fd) : client_fd(fd), requestInfo(info) {}

Response::~Response(void) {}

// MAIN METHOD
void	Response::treatActionAndResponse(void)
{
	Post postHandler;
	Delete deleteHandler;
	Get getHandler(*this);
	switch (requestInfo.action)
	{
		case RESPONSE:
			getHandler.handler();
			break ;
		case UPLOAD:
			postHandler.handle(*this);
			break ;
		case DELETE:
			deleteHandler.handle(*this);
			break ;
		case CLOSE:
			return ;
	}
	sendResponse();
}

void Response::setStatusLine(int statusCode, const std::string& reasonPhrase)
{
	std::ostringstream statusStream;
	statusStream << "HTTP/1.1 " << statusCode << " " << reasonPhrase;
	responseMsg.statusLine = statusStream.str();
}

void Response::setHeader(const std::string& key, const std::string& value) {
	responseMsg.headers[key] = value;
}

std::string Response::setBodyFromFile(const std::string& bodyFile)
{
	std::string ret;

	std::ifstream file(bodyFile.c_str());
	if (!file)
		return NOT_FOUND_ERROR;
	std::ostringstream oss;
	oss << file.rdbuf();
	ret = oss.str();
	file.close();
	return ret;
}

void Response::setBodyFromDefaultPage(const std::string& defaultPage)
{
	responseMsg.body = defaultPage;
}

std::string Response::buildResponse(void)
{
	std::ostringstream responseStream;
	responseStream << responseMsg.statusLine << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = responseMsg.headers.begin(); it != responseMsg.headers.end(); ++it){
		responseStream << it->first << ": " << it->second << "\r\n";
	}
	responseStream << "\r\n" << responseMsg.body;
	return responseStream.str();
}

// UTILITY METHODS

std::string Response::getStatusMessage(int statusCode)
{
	switch (statusCode)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 400: return "Bad Request";
		case 403: return "Forbidden";
		case 404: return "Not Found";
		case 405: return "Method Not Allowed";
		case 408: return "Request Timeout";
		case 409: return "Conflict";
		case 413: return "Payload Too Large";
		case 500: return "Internal Server Error";
		case 501: return "Not Implemented";
		case 503: return "Service Unavailable";
		default: return "Internal Server Error";
	}
}

std::string Response::getDefaultPage(int statusCode)
{
	switch (statusCode)
	{
		case 204: return NO_CONTENT;
		case 400: return BAD_REQUEST_ERROR;
		case 403: return FORBIDDEN_ERROR;
		case 404: return NOT_FOUND_ERROR;
		case 405: return METHOD_NOT_ALLOWED_ERROR;
		case 408: return REQUEST_TIMEOUT_ERROR;
		case 409: return CONFLICT_ERROR;
		case 413: return PAYLOAD_TOO_LARGE_ERROR;
		case 500: return INTERNAL_SERVER_ERROR;
		case 501: return NOT_IMPLEMENTED_ERROR;
		case 503: return SERVICE_UNAVAILABLE_ERROR;
		default: return NO_DEFAULT_ERROR;
	}
}

// PUBLIC METHODS

void Response::setResponseMsg(int statusCode, std::string const &htmlFile)
{
	std::string statusMessage = getStatusMessage(statusCode);
	std::string body = getDefaultPage(statusCode);

	setStatusLine(statusCode, statusMessage);
	setHeader("Content-Type", "text/html");

	if (requestInfo.pathType == CGI)
		body = htmlFile;
	else if (body == NO_DEFAULT_ERROR)
		body = setBodyFromFile(htmlFile);
	setBodyFromDefaultPage(body);

	// Get response size;
	std::ostringstream sizeStream;
	sizeStream << responseMsg.body.size();
	setHeader("Content-Length", sizeStream.str());
}

// SENDING
void Response::sendResponse(void)
{
	std::string response = buildResponse();
	// std::cout << " ***** sendResponse *****\n"
	// 			<< "fd:" << client_fd
	// 			<< "\n\n-------reponse-----\n"
	// 			<< response
	// 			<< "\n-----------------------"
	// 			<< std::endl;
	send(client_fd, response.c_str(), response.size(), 0);
}
