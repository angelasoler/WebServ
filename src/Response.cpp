#include "Response.hpp"
#include "Request.hpp"
#include <sys/socket.h>
#include <unistd.h>

Response::Response(void) {}

Response::~Response(void) {}

void Response::setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase) {
	std::ostringstream statusStream;
	statusStream << version << " " << statusCode << " " << reasonPhrase;
	statusLine = statusStream.str();
}

void Response::setHeader(const std::string& key, const std::string& value) {
	headers[key] = value;
}

void Response::setBody(const std::string& bodyFile) {
    std::ifstream file(bodyFile.c_str());
    if (!file) {
        throw std::runtime_error("Could not open file: " + bodyFile);
    }
    std::ostringstream oss;
    oss << file.rdbuf();
    body = oss.str();
}

std::string Response::buildResponse() {
	std::ostringstream responseStream;
	responseStream << statusLine << "\r\n";
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it) {
		responseStream << it->first << ": " << it->second << "\r\n";
	}
	responseStream << "\r\n" << body;
	return responseStream.str();
}

void Response::sendResponse(int client_fd) {
	std::string response = buildResponse();
	send(client_fd, response.c_str(), response.size(), 0);
}

int Response::treatActionAndResponse(std::map<int, std::string> request, int client_fd, e_httpMethodActions action) {
	if (!request.empty() && !request[client_fd].empty()) {
		switch (action) {
			case RESPONSE:
				setStatusLine("HTTP/1.1", 200, "OK");
				setHeader("Content-Type", "text/html");
				setHeader("Content-Length", "1024");
				setBody("helloworld.html");
				sendResponse(client_fd);
				break;
			case CLOSE:
				request.erase(request.find(client_fd));
				close(client_fd);
				break;
		}
		return 1;
	}
	return 0;
}
