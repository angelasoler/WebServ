
#ifndef REQUEST_HPP
#define REQUEST_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

typedef enum
{
	RESPONSE,
	CLOSE
}	e_httpMethodActions;

# include <iostream>
# include <unistd.h>
# include <vector>
# include <arpa/inet.h>
# include <poll.h>
# include <fcntl.h>
# include <cstring>
# include <algorithm>
# include <map>

class Request
{
	private:
	public:
		Request(void);
		~Request(void);

		e_httpMethodActions	parseRequest(std::string text);
		int					readRequest(int client_fd, \
							std::map<int, std::string> &request);
};

#endif /* REQUEST_HPP */
