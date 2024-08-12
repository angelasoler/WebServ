#include "Response.hpp"
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include <sys/socket.h>
#include <unistd.h>

// CONSTRUCTOR AND DESTRUCTOR
Response::Response(void) {}

Response::~Response(void) {}

// MAIN METHOD
int Response::treatActionAndResponse(int client_fd, RequestInfo &requestInfo)
{
	switch (requestInfo.action)
	{
		case RESPONSE:
			getHandler.handle(requestInfo, *this);
			break;
		case UPLOAD:
			postHandler.handle(requestInfo, *this);
			break;
		case DELETE:
			deleteHandler.handle(requestInfo, *this);
			break;
		case CLOSE:
			break;
	}
	sendResponse(client_fd);
	return 1;
}


// INTERNAL METHODS

void Response::setStatusLine(int statusCode, const std::string& reasonPhrase)
{
	std::ostringstream statusStream;
	statusStream << "HTTP/1.1 " << statusCode << " " << reasonPhrase;
	responseMsg.statusLine = statusStream.str();
}

void Response::setHeader(const std::string& key, const std::string& value) {
	responseMsg.headers[key] = value;
}

void Response::setBodyFromFile(const std::string& bodyFile)
{
	std::ifstream file(bodyFile.c_str());
	if (!file) {
		setBodyFromDefaultPage(NOT_FOUND_ERROR);
		return;
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	responseMsg.body = oss.str();
	file.close();
}

void Response::setBodyFromDefaultPage(const std::string& defaultPage)
{
	responseMsg.body = defaultPage;
}

std::string Response::buildResponse()
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
	std::string defaultPage = getDefaultPage(statusCode);

	setStatusLine(statusCode, statusMessage);
	setHeader("Content-Type", "text/html");

	if (defaultPage == NO_DEFAULT_ERROR)
		setBodyFromFile(htmlFile);
	else
		setBodyFromDefaultPage(defaultPage);

	// Get response size;
	std::ostringstream sizeStream;
	sizeStream << responseMsg.body.size();
	setHeader("Content-Length", sizeStream.str());
}

// SENDING
void Response::sendResponse(int client_fd)
{
	std::string response = buildResponse();
	send(client_fd, response.c_str(), response.size(), 0);
}
