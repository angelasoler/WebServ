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

		int handleRequest(void);
	private:
		Response &response;

		int			upload(void);
		void		buildBody(void);
		// std::string
};

#endif // POST_HPP
