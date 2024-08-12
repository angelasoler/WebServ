#ifndef POST_HPP
# define POST_HPP

# include "Request.hpp"

class Response;

class Post
{
	public:
		void handle(RequestInfo &requestInfo, Response &response);
	private:
		void upload(RequestInfo &requestInfo, Response &response);
};

#endif // POST_HPP
