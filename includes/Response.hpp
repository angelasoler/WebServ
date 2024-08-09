#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include <string>
#include <fstream>
#include <sstream>
# include "Server.hpp"
# include "Config.hpp"
# define NOT_FOUND_ERROR "<html><head><title>ERROR 404</title></head><body><h1>ERROR 404<br>Page not found!</h1></body></html>"
# define BAD_REQUEST_ERROR "<html><head><title>ERROR 400</title></head><body><h1>ERROR 400<br>BAD REQUEST!</h1></body></html>"
# define FORBIDDEN_ERROR "<html><head><title>ERROR 403</title></head><body><h1>ERROR 403<br>FORBIDDEN!</h1></body></html>"
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

		// PARSING
		void		setBody(const std::string& bodyFile);
		void		setBodyError(const std::string& bodyError);
		std::string	buildResponse(void);
		void		setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase);
		void		setHeader(const std::string& key, const std::string& value);
	
		// ACTIONS
		void		response(int client_fd, RequestInfo &requestInfo);
		void		responseToFile(int client_fd, RequestInfo &requestInfo);
		void		responseToInvalid(int client_fd, RequestInfo &requestInfo);

		// SEND RESPONSE
		void		sendResponse(int client_fd);

		// COPY ASSINGMENT
		Response	&operator=(const Response &cpy);
	public:
		Response();
		~Response(void);
		int			treatActionAndResponse(int client_fd, RequestInfo &requestInfo);
};

#endif /* RESPONSE_HPP */
