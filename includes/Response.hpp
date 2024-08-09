#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include <string>
#include <fstream>
#include <sstream>
# include "Server.hpp"
# include "Config.hpp"
# define NO_CONTENT "<html><head><title>204</title></head><body><h1>204<br>No Content!</h1></body></html>"
# define BAD_REQUEST_ERROR "<html><head><title>ERROR 400</title></head><body><h1>ERROR 400<br>Bad Request!</h1></body></html>"
# define FORBIDDEN_ERROR "<html><head><title>ERROR 403</title></head><body><h1>ERROR 403<br>Forbidden!</h1></body></html>"
# define NOT_FOUND_ERROR "<html><head><title>ERROR 404</title></head><body><h1>ERROR 404<br>Page not found!</h1></body></html>"
# define METHOD_NOT_ALLOWED_ERROR "<html><head><title>ERROR 405</title></head><body><h1>ERROR 405<br>Method Not Allowed!</h1></body></html>"
# define REQUEST_TIMEOUT_ERROR "<html><head><title>ERROR 408</title></head><body><h1>ERROR 408<br>Request Timeout!</h1></body></html>"
# define CONFLICT_ERROR "<html><head><title>ERROR 409</title></head><body><h1>ERROR 409<br>Conflict!</h1></body></html>"
# define PAYLOAD_TOO_LARGE_ERROR "<html><head><title>ERROR 413</title></head><body><h1>ERROR 413<br>Payload Too Large!</h1></body></html>"
# define INTERNAL_SERVER_ERROR "<html><head><title>ERROR 500</title></head><body><h1>ERROR 500<br>Internal Server Error!</h1></body></html>"
# define NOT_IMPLEMENTED_ERROR "<html><head><title>ERROR 501</title></head><body><h1>ERROR 501<br>Not Implemented!</h1></body></html>"
# define SERVICE_UNAVAILABLE_ERROR "<html><head><title>ERROR 503</title></head><body><h1>ERROR 503<br>Service Unavailable!</h1></body></html>"

# include <iostream>
# include <algorithm>
# include <map>

struct ResponseMsg
{
	std::string							statusLine;
	std::map<std::string, std::string>	headers;
	std::string							body;
};

class Response
{
	private:
		ResponseMsg	responseMsg;

		// PARSING AND SENDING
		void		setBody(const std::string& bodyFile);
		void		setBodyError(const std::string& bodyError);
		std::string	buildResponse(void);
		void		setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase);
		void		setHeader(const std::string& key, const std::string& value);
		void		setResponse(int statusCode, std::string htmlFile);

		// SEND RESPONSE
		void		sendResponse(int client_fd);

		// ACTIONS
		// RESPONSE
		void		response(int client_fd, RequestInfo &requestInfo);
		void		responseToFile(int client_fd, RequestInfo &requestInfo);
		void		responseToDirectory(int client_fd, RequestInfo &requestInfo);
		void		responseToInvalid(int client_fd, RequestInfo &requestInfo);
		// UPLOAD
		void		upload(int client_fd, RequestInfo &requestInfo);
		// DELETE
		void		deleteAction(int client_fd, RequestInfo &requestInfo);
		bool		deleTeDirectory(RequestInfo &requestInfo);

		// COPY ASSINGMENT
		Response	&operator=(const Response &cpy);
	public:
		Response();
		~Response(void);
		int			treatActionAndResponse(int client_fd, RequestInfo &requestInfo);
};

#endif /* RESPONSE_HPP */
