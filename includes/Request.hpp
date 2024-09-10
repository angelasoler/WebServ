
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
# include "RequestReader.hpp"

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
	CLOSE,
	AWAIT_READ,
	AWAIT_WRITE
}	e_httpMethodActions;

typedef enum
{
	File,
	Directory,
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
	std::string							requestedRoute;
	std::string							fullPath;
	e_pathType							pathType;
	Permission							permissions;

	std::string				queryString;

	// Reference
	ServerConfig			serverRef;
	RouteConfig				routeRef;

	// About Body
	std::string							contentType;
	e_httpMethodActions					action;
	std::vector<char>					rawBody;
	std::string							body;
	std::vector<std::string>			multipartHeaders;
	std::vector<std::string>			multipartValues;
	std::map< std::string, std::string>	urlencodedValues;

	RequestInfo();
};

class Request
{
	private:
		std::map< std::string, std::vector<std::string> >	header;

		// reading
		bool	isComplete(const std::vector<char>& clientRequestText);
		ssize_t	findContentLength(const std::vector<char>& clientRequestText);
		bool	findEndRequest(const std::vector<char> &vec);

		// debug
		void	printRequest(void);

		// Parsing
		void	parseRequestInfo(ServerConfig &serverConfig);

		// Aux Parsing
		e_httpMethodActions									getMethodAction(void);
	public:
		std::string			requestsText;
		std::vector<char>	requestVec;
		RequestReader		requestReader;
		RequestInfo			info;

		Request(void);
		~Request(void);
		void				parseRequest(ServerConfig &serverConfig);
		bool				readRequest(int client_fd);
		int					getInformationalStatus(void);
};

#endif /* REQUEST_HPP */