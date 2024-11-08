#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"

#include "Get.hpp"
#include "Delete.hpp"
#include "Post.hpp"

#include "ParsePathInfo.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "CGIServer.hpp"
# include "IHttpMethod.hpp"

#define CREATED_SUCCESSFULLY "<html><head><title>201</title></head><body><h1>201<br>Created Successfully!</h1></body></html>"
#define NO_CONTENT "<html><head><title>204</title></head><body><h1>204<br>No Content!</h1></body></html>"
#define BAD_REQUEST_ERROR "<html><head><title>ERROR 400</title></head><body><h1>ERROR 400<br>Bad Request!</h1></body></html>"
#define FORBIDDEN_ERROR "<html><head><title>ERROR 403</title></head><body><h1>ERROR 403<br>Forbidden!</h1></body></html>"
#define NOT_FOUND_ERROR "<html><head><title>ERROR 404</title></head><body><h1>ERROR 404<br>Page not found!</h1></body></html>"
#define METHOD_NOT_ALLOWED_ERROR "<html><head><title>ERROR 405</title></head><body><h1>ERROR 405<br>Method Not Allowed!</h1></body></html>"
#define REQUEST_TIMEOUT_ERROR "<html><head><title>ERROR 408</title></head><body><h1>ERROR 408<br>Request Timeout!</h1></body></html>"
#define CONFLICT_ERROR "<html><head><title>ERROR 409</title></head><body><h1>ERROR 409<br>Conflict!</h1></body></html>"
#define PAYLOAD_TOO_LARGE_ERROR "<html><head><title>ERROR 413</title></head><body><h1>ERROR 413<br>Payload Too Large!</h1></body></html>"
#define INTERNAL_SERVER_ERROR "<html><head><title>ERROR 500</title></head><body><h1>ERROR 500<br>Internal Server Error!</h1></body></html>"
#define NOT_IMPLEMENTED_ERROR "<html><head><title>ERROR 501</title></head><body><h1>ERROR 501<br>Not Implemented!</h1></body></html>"
#define SERVICE_UNAVAILABLE_ERROR "<html><head><title>ERROR 503</title></head><body><h1>ERROR 503<br>Service Unavailable!</h1></body></html>"
#define	NO_DEFAULT_ERROR ""

struct ResponseMsg
{
	std::string statusLine;
	std::map<std::string, std::string> headers;
	std::string body;
};

class Response
{
	private:
		std::map<std::string, std::string> mimeTypes;

		ResponseMsg	responseMsg;
		int			client_fd;
		int			statusCode;
		std::string	response;
		size_t		bytesSent;

		// PARSING
		std::string	buildResponse(void);
		void		setStatusLine(void);
		void		setResponseMsg(IHttpMethod *method);

		// UTILITY
		std::string	getStatusMessage(void);

		// SEND RESPONSE
		void	printResponse(std::string &response);
		void	buildContentType(void);

		// log
		void 	sendLogs(int ret, size_t bytesSent, size_t bytesToSend);
	public:
		RequestInfo	requestInfo;

		Response();
		Response(RequestInfo info, int fd);
		~Response(void);

		int			getClientFd(void);
		void		treatActionAndResponse(void);
		void		setBody(const std::string& body);
		std::string	&getBody(void);
		void		setHeader(const std::string& key, const std::string& value);
		std::string	getDefaultPage(void);
		void		sendResponse(void);
		void		setResponseStr(void);
};

#endif /* RESPONSE_HPP */
