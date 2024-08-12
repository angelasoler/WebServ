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
			getHandler.handle(client_fd, requestInfo, *this);
			break;
		case UPLOAD:
			postHandler.handle(client_fd, requestInfo, *this);
			break;
		case DELETE:
			deleteHandler.handle(client_fd, requestInfo, *this);
			break;
		case CLOSE:
			break;
	}
	return 1;
}

// PARSE AND SENDING RESPONSE

void Response::setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase) {
	std::ostringstream statusStream;
	statusStream << version << " " << statusCode << " " << reasonPhrase;
	responseMsg.statusLine = statusStream.str();
}

void Response::setHeader(const std::string& key, const std::string& value) {
	responseMsg.headers[key] = value;
}

void Response::setBody(const std::string& bodyFile) {
	std::ifstream file(bodyFile.c_str());
	if (!file) {
		setBodyError(NOT_FOUND_ERROR);
		return ;
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	responseMsg.body = oss.str();
	file.close();
}

void Response::setBodyError(const std::string& bodyError)
{
	responseMsg.body = bodyError;
}

std::string Response::buildResponse() {
	std::ostringstream responseStream;
	responseStream << responseMsg.statusLine << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = responseMsg.headers.begin(); it != responseMsg.headers.end(); ++it) {
		responseStream << it->first << ": " << it->second << "\r\n";
	}
	responseStream << "\r\n" << responseMsg.body;
	return responseStream.str();
}

void Response::setResponse(int statusCode, std::string htmlFile)
{
	std::string statusMessage;
	std::string errorBody;

	switch (statusCode) {
		case 200:
			statusMessage = "OK";
			break;
		case 201:
			statusMessage = "Created";
			break;
		case 204:
			statusMessage = "No Content";
			errorBody = NO_CONTENT;
			break;
		case 301:
			statusMessage = "Moved Permanently";
			break;
		case 400:
			statusMessage = "Bad Request";
			errorBody = BAD_REQUEST_ERROR;
			break;
		case 403:
			statusMessage = "Forbidden";
			errorBody = FORBIDDEN_ERROR;
			break;
		case 404:
			statusMessage = "Not Found";
			errorBody = NOT_FOUND_ERROR;
			break;
		case 405:
			statusMessage = "Method Not Allowed";
			errorBody = METHOD_NOT_ALLOWED_ERROR;
			break;
		case 408:
			statusMessage = "Request Timeout";
			errorBody = REQUEST_TIMEOUT_ERROR;
			break;
		case 409:
			statusMessage = "Conflict";
			errorBody = CONFLICT_ERROR;
			break;
		case 413:
			statusMessage = "Payload Too Large";
			errorBody = PAYLOAD_TOO_LARGE_ERROR;
			break;
		case 500:
			statusMessage = "Internal Server Error";
			errorBody = INTERNAL_SERVER_ERROR;
			break;
		case 501:
			statusMessage = "Not Implemented";
			errorBody = NOT_IMPLEMENTED_ERROR;
			break;
		case 503:
			statusMessage = "Service Unavailable";
			errorBody = SERVICE_UNAVAILABLE_ERROR;
			break;
		default:
			statusCode = 500;
			statusMessage = "Internal Server Error";
			errorBody = INTERNAL_SERVER_ERROR;
			break;
	}

	setStatusLine("HTTP/1.1", statusCode, statusMessage);
	setHeader("Content-Type", "text/html");

	if (!errorBody.empty()) {
		setBodyError(errorBody);
	} else {
		setBody(htmlFile);
	}

	// Get response size;
	std::ostringstream sizeStream;
	sizeStream << responseMsg.body.size();
	setHeader("Content-Length", sizeStream.str());
}

void Response::sendResponse(int client_fd) {
	std::string response = buildResponse();
	send(client_fd, response.c_str(), response.size(), 0);
}
