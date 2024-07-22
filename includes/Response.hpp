
#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
# include <algorithm>
# include <map>

class Response
{
	private:
	public:
		Response(void);
		~Response(void);

		void	sendResponse(int client_fd);
		int		treatActionAndResponse(std::map<int, std::string> request, int client_fd, e_httpMethodActions action);
		
};

#endif /* RESPONSE_HPP */
