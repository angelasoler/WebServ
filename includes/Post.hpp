#ifndef POST_HPP
# define POST_HPP

# include "Request.hpp"
# include "IHttpMethod.hpp"

class Response;

class Post : public IHttpMethod
{
	public:
		Post(Response &objectRef);
		~Post(void);

		void handleRequest(void);
	private:
		Response &response;

		void upload(void);
};

#endif // POST_HPP
