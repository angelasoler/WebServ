#include "Response.hpp"
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include <sys/socket.h>
#include <unistd.h>
# include "TimeNow.hpp"

Response::Response(RequestInfo info, int fd) : client_fd(fd), requestInfo(info)
{
	mimeTypes["html"]	= "text/html";
	mimeTypes["htm"]	= "text/html";
	mimeTypes["css"]	= "text/css";
	mimeTypes["js"]		= "text/javascript";
	mimeTypes["mjs"]	= "text/javascript";
	mimeTypes["json"]	= "application/json";
	mimeTypes["php"]	= "application/x-httpd-php";
	mimeTypes["xml"]	= "application/xml";
	mimeTypes["jpeg"]	= "image/jpeg";
	mimeTypes["jpg"]	= "image/jpeg";
	mimeTypes["png"]	= "image/png";
	mimeTypes["gif"]	= "image/gif";
	mimeTypes["svg"]	= "image/svg+xml";
	mimeTypes["mp3"]	= "audio/mpeg";
	mimeTypes["wav"]	= "audio/wav";
	mimeTypes["ogg"]	= "audio/ogg";
	mimeTypes["mp4"]	= "video/mp4";
	mimeTypes["webm"]	= "video/webm";
	mimeTypes["pdf"]	= "application/pdf";
	mimeTypes["zip"]	= "application/zip";
}

Response::~Response(void) {}

// MAIN METHOD
void	Response::treatActionAndResponse(void)
{
	IHttpMethod	*method;

	switch (requestInfo.action)
	{
		case RESPONSE:
			method = new Get(*this);
			break ;
		case UPLOAD:
			method = new Post(*this);
			break ;
		case DELETE:
			method = new Delete(*this);
			break ;
		case CLOSE:
			return ;
	}
	statusCode = method->handleRequest();
	setResponseMsg(method);
	sendResponse();
	delete method;
}

void Response::setStatusLine(void)
{
	const std::string& reasonPhrase = getStatusMessage();

	std::ostringstream statusStream;
	statusStream << "HTTP/1.1 " << statusCode << " " << reasonPhrase;
	responseMsg.statusLine = statusStream.str();
}

void Response::setHeader(const std::string& key, const std::string& value) {
	responseMsg.headers[key] = value;
}

void Response::setBody(const std::string& body)
{
	responseMsg.body = body;
}

std::string &Response::getBody(void)
{
	return responseMsg.body;
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

std::string Response::getStatusMessage(void)
{
	switch (statusCode)
	{
		case 200: return "OK";
		case 201: return "Created";
		case 204: return "No Content";
		case 307: return "Temporary Redirect";
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

std::string Response::getDefaultPage(void)
{
	if (requestInfo.serverRef.default_error_page.find(statusCode) != requestInfo.serverRef.default_error_page.end() && \
		!requestInfo.serverRef.default_error_page[statusCode].empty()) {
		std::string content = getBodyFromFile(requestInfo.serverRef.default_error_page[statusCode]);
		return (content);
	}
	switch (statusCode)
	{
		case 201: return CREATED_SUCCESSFULLY;
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
void	Response::buildContentType(void)
{
	size_t		dot_position;

	if (requestInfo.pathType != File)
	{
		setHeader("Content-Type", "text/html; charset=utf-8");
		return ;
	}

	dot_position = requestInfo.fullPath.find_last_of(".");

	if (dot_position == std::string::npos || requestInfo.fullPath.find_last_of("/") > dot_position)
	{
		setHeader("Content-Type", "text/plain; charset=utf-8");
		return ;
	}

	std::string value;
	std::string extension = requestInfo.fullPath.substr(dot_position + 1);

	std::map< std::string, std::string >::iterator type;

	type = mimeTypes.find(extension);
	if (type != mimeTypes.end())
		value = type->second + ";charset=utf-8";
	else
		value = "text/plain; charset=utf-8";
	setHeader("Content-Type", value);
}

void Response::setResponseMsg(IHttpMethod *method)
{
	setStatusLine();
	buildContentType();
	method->buildBody();

	// Get response size;
	std::ostringstream sizeStream;
	sizeStream << responseMsg.body.size();
	setHeader("Content-Length", sizeStream.str());
}

void	Response::printResponse(std::string &response)
{
	std::ofstream	responseLog("logs/response.log", std::ios_base::app);

	responseLog << std::endl << TimeNow();
	responseLog << "\nfd:" << client_fd
				<< "\n-------reponse-----\n"
				<< response
				<< "\n-----------------------"
				<< std::endl;
	responseLog.close();
}

// SENDING
void Response::sendResponse(void)
{
	std::string response = buildResponse();
	printResponse(response);
	int ret = send(client_fd, response.c_str(), response.size(), 0);
	if (ret == -1)
		requestInfo.action = CLOSE;
}

int			Response::getClientFd(void)
{
	return this->client_fd;
}