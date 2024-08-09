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
	switch (requestInfo.action) {
		case RESPONSE:
			response(client_fd, requestInfo);
			break;
		case UPLOAD:
			upload(client_fd, requestInfo);
			break;
		case DELETE:
			break;
		case CLOSE:
			break;
	}
	return 1;
}


// RESPONSE
void	Response::response(int client_fd, RequestInfo &requestInfo)
{
	if (requestInfo.pathType == File || requestInfo.pathType == URL)
		responseToFile(client_fd, requestInfo);
	else if (requestInfo.pathType == Directory)
		responseToDirectory(client_fd, requestInfo);
	else
		responseToInvalid(client_fd, requestInfo);
}

void	Response::responseToFile(int client_fd, RequestInfo &requestInfo)
{
	// std::cout << "fullpath: " << requestInfo.fullPath << "\n";
	if (!requestInfo.permissions.read) {
		setResponse(403, FORBIDDEN_ERROR);
	}
	else if (!requestInfo.fullPath.empty()) {
		setResponse(200, requestInfo.fullPath);
	} else {
		setResponse(404, NOT_FOUND_ERROR);
	}
	sendResponse(client_fd);
}

void	Response::responseToDirectory(int client_fd, RequestInfo &requestInfo)
{
	// // std::cout << "fullpath: " << requestInfo.fullPath << "\n";
	if (!endsWith(requestInfo.requestedRoute, "/")) { // ainda nao entendi isso corretamente !
		setResponse(301, requestInfo.fullPath);
	}
	else if (!requestInfo.fullPath.empty()) {
		setResponse(200, requestInfo.fullPath);
	}
	else if (requestInfo.auto_index) {
		// return auto-index of directiory
	}
	else
		setResponse(403, FORBIDDEN_ERROR);
	sendResponse(client_fd);
}

void	Response::responseToInvalid(int client_fd, RequestInfo &requestInfo)
{
	(void)requestInfo;
	// std::cout << "Bad Request: " << requestInfo.requestedRoute << "\n";
	setResponse(400, BAD_REQUEST_ERROR);
	sendResponse(client_fd);
}

// UPLOAD | POST
void	Response::upload(int client_fd, RequestInfo &requestInfo)
{
	(void)client_fd;
	(void)requestInfo;
}

// DELETE
void	Response::deleteAction(int client_fd, RequestInfo &requestInfo)
{
	if (requestInfo.pathType == File) {
		// Delete File Funcion
		setResponse(204, NO_CONTENT);
	}
	else if (!endsWith(requestInfo.requestedRoute, "/")) { // ainda nao entendi isso corretamente !
		setResponse(409, CONFLICT_ERROR);
	}
	else if (!requestInfo.permissions.write) {
		setResponse(403, FORBIDDEN_ERROR);
	}
	else if (deleTeDirectory(requestInfo)) {
		setResponse(204, NO_CONTENT);
	}
	else {
		setResponse(500, INTERNAL_SERVER_ERROR);
	}
	sendResponse(client_fd);
}

bool	Response::deleTeDirectory(RequestInfo &requestInfo)
{
	(void)requestInfo;
	// Try to delete a Directory
	return (true);
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
