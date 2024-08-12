#ifndef POST_HPP
# define POST_HPP

# include "Request.hpp"

class Response;

class Post
{
	public:
		void handle(int client_fd, RequestInfo &requestInfo, Response &response);
	private:
		void upload(int client_fd, RequestInfo &requestInfo, Response &response);
};

#endif // POST_HPP
