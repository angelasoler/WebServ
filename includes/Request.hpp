
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
	bool	notFound;
};

struct RequestInfo
{
	std::string				requestedRoute;
	std::string				fullPath;
	e_pathType				pathType;
	e_httpMethodActions		action;
	Permission				permissions;
	bool					auto_index;
	// Reference
	ServerConfig			serverRef;
	RouteConfig				configRef;

	// About Body
	std::string							boundary;
	std::string							body;
	std::string							contentType;
	std::map< std::string, std::string>	bodyValues;
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
		std::vector<char>	requestsVector;
		std::string	requestsText;
		RequestInfo	info;

		bool		requestCompleted;
		Request(void);
		~Request(void);
		void	parseRequest(ServerConfig &serverConfig);
		bool	readRequest(int client_fd);
		void	cleanHeader(void);
};

#endif /* REQUEST_HPP */
