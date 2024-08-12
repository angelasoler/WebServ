#ifndef GET_HPP
# define GET_HPP

# include "Request.hpp"
class Response;

class Get
{
	public:
		void handle(int client_fd, RequestInfo &requestInfo, Response &response);
	private:
		void responseToFile(int client_fd, RequestInfo &requestInfo, Response &response);
		void responseToDirectory(int client_fd, RequestInfo &requestInfo, Response &response);
		void responseToInvalid(int client_fd, RequestInfo &requestInfo, Response &response);
};

#endif // GET_HPP
