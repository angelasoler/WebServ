
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
	Missing,
	CGI
}	e_pathType;

typedef enum
{
	Read,
	Write,
	Execute
}	e_permission;

struct RequestInfo
{
	std::string					requestPath;
	std::string					ruquestCompletePath;
	e_pathType					pathType;
	e_httpMethodActions			action;
	e_permission				permissions;
	ServerConfig				serverConfig;
};

class Request
{
	private:
		std::map< std::string, std::vector<std::string> >	header;

		void	parseTheOthers(std::vector<std::string> &lines);
		void	printHeaderDataStructure(void);
		void	breakResquesLine(std::string &line);
		e_httpMethodActions	getMethodAction(void);
		RequestInfo			findServerConfig(int client_fd);
	public:
		Request(void);
		~Request(void);

		RequestInfo 										parseRequest(std::string text, ServerConfig &serverConfig);
		void												parseRequestInfo(ServerConfig &serverConfig, RequestInfo &info);
		void												parseRequestHeader(std::string text);
		int													readRequest(int client_fd, \
																		std::map<int, std::string> &request);
		std::map< std::string, std::vector<std::string> >	&getHeader(void);
		void												cleanHeader(void);

};

#endif /* REQUEST_HPP */
