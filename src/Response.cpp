#include "Response.hpp"
#include "Request.hpp"
#include <sys/socket.h>
#include <unistd.h>

Response::Response(void) {}

Response::~Response(void) {}

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
		responseMsg.body = NOT_FOUND_PAGE_ERROR;
		return ;
	}
	std::ostringstream oss;
	oss << file.rdbuf();
	responseMsg.body = oss.str();
	file.close();
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

void Response::sendResponse(int client_fd) {
	std::string response = buildResponse();
	send(client_fd, response.c_str(), response.size(), 0);
}

int Response::treatActionAndResponse(int client_fd, RequestInfo &requestInfo)
{
	switch (requestInfo.action) {
		case RESPONSE:
			response(client_fd, requestInfo);
			break;
		case UPLOAD:
			break;
		case DELETE:
			break;
		case CLOSE:
			break;
	}
	return 1;
}

void	Response::response(int client_fd, RequestInfo &requestInfo)
{
	std::cout << "fullpath: " << requestInfo.fullPath << "\n";
	if (!requestInfo.fullPath.empty()) {
		
		setStatusLine("HTTP/1.1", 200, "OK");
		setHeader("Content-Type", "text/html");
		setBody(requestInfo.fullPath);
		
	} else {
		setStatusLine("HTTP/1.1", 404, "OK");
		setHeader("Content-Type", "text/html");
		setBody(requestInfo.serverRef.default_error_page);
	}

	std::ostringstream sizeStream;
	sizeStream << responseMsg.body.size();
	setHeader("Content-Length", sizeStream.str());
	sendResponse(client_fd);
}
