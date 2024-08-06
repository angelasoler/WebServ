#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include <string>
#include <fstream>
#include <sstream>
# include "Server.hpp"
# include "Config.hpp"
# define NOT_FOUND_PAGE_ERROR "<html><head><title>ERROR</title></head><body><h1>Page not found!</h1></body></html>"
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

		// PÁRSING
		void		setBody(const std::string& bodyFile);
		std::string	buildResponse(void);
		void		setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase);
		void		setHeader(const std::string& key, const std::string& value);
	
		// ACTIONS
		void		response(int client_fd, ServerConfig &serverConfig, RequestInfo &requestInfo);

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
