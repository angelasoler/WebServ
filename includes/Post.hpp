#ifndef POST_HPP
# define POST_HPP

# include "Request.hpp"

class Response;

class Post
{
	public:
		void handle(Response &response);
	private:
		void upload(Response &response);
};

#endif // POST_HPP
