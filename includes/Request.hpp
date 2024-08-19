
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
	METHOD,
	ROUTE,
	HTTP_VERSION
}	e_requestLinePart;

typedef enum
{
	RESPONSE,
	UPLOAD,
	DELETE,
	CLOSE
}	e_httpMethodActions;

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
	std::string				requestedRoute;
	std::string				fullPath;
	e_pathType				pathType;
	e_httpMethodActions		action;
	Permission				permissions;
	std::string				body;
	bool					auto_index;
	// Reference
	ServerConfig			serverRef;

	// Request
	std::string				contentType;
	std::map< std::string, std::string>	requestBody;
};

class Request
{
	private:
		std::map< std::string, std::vector<std::string> >	header;

		// debug
		void	printHeaderDataStructure(void);
		void	printRequest(void);

		// Parsing
		void	parseTheOthers(std::vector<std::string> &lines);
		void	breakResquesLine(std::string &line);
		void	parseRequestInfo(ServerConfig &serverConfig);
		void	parseRequestHeader(void);
		void	breakIntoLines(std::vector<std::string> &lines);

		// Aux Parsing
		e_httpMethodActions									getMethodAction(void);
		std::map< std::string, std::vector<std::string> >	&getHeader(void);
	public:
		std::string	requestsText;
		RequestInfo	info;

		Request(void);
		~Request(void);
		void	parseRequest(ServerConfig &serverConfig);
		void	readRequest(int client_fd);
		void	cleanHeader(void);
};

#endif /* REQUEST_HPP */
