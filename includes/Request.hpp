
#ifndef REQUEST_HPP
#define REQUEST_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

# include <iostream>
# include <unistd.h>
# include <vector>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <cstring>
# include <algorithm>
# include <map>
# include <sstream>
# include <string.h>
# include <string>
# include "Config.hpp"

typedef enum
{
	RESPONSE,
	UPLOAD,
	DELETE,
	CLOSE
}	e_httpMethodActions;

typedef enum
{
	METHOD,
	ROUTE,
	HTTP_VERSION
}	e_requestLinePart;

typedef enum
{
	File,
	Directory,
	URL,
	Redirection,
	CGI,
	UNKNOWN
}	e_pathType;

struct Permission
{
	bool	read;
	bool	write;
	bool	execute;
};

struct RequestInfo
{
	std::string					requestedRoute;
	std::string					fullPath;
	e_pathType					pathType;
	e_httpMethodActions			action;
	Permission					permissions;
	std::string					body;

	// Reference
	ServerConfig				serverRef;
};

class Request
{
	private:
		std::map< std::string, std::vector<std::string> >	header;

		// debug
		void				printHeaderDataStructure(void);

		// Parsing
		void				parseTheOthers(std::vector<std::string> &lines);
		void				breakResquesLine(std::string &line);
		void				parseRequestInfo(ServerConfig &serverConfig, RequestInfo &info);
		void				parseRequestHeader(std::string text);

		// Aux Parsing
		e_httpMethodActions									getMethodAction(void);
		std::map< std::string, std::vector<std::string> >	&getHeader(void);
	public:
		Request(void);
		~Request(void);
		RequestInfo 										parseRequest(std::string text, ServerConfig &serverConfig);
		int													readRequest(int client_fd, \
																		std::map<int, std::string> &request);
		void												cleanHeader(void);

};

#endif /* REQUEST_HPP */
