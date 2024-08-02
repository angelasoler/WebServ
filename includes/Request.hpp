
#ifndef REQUEST_HPP
#define REQUEST_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

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

class Request
{
	private:
		std::map< std::string, std::vector<std::string> >	header;

		void	parseTheOthers(std::vector<std::string> &lines);
		void	dataStrcuture(std::string text);
		void	printHeaderDataStructure(void);
		void	breakResquesLine(std::string &line);
	public:
		Request(void);
		~Request(void);

		e_httpMethodActions									parseRequest(std::string text);
		int													readRequest(int client_fd, \
																		std::map<int, std::string> &request);
		std::map< std::string, std::vector<std::string> >	&getHeader(void);
		void												cleanHeader(void);

};

#endif /* REQUEST_HPP */
