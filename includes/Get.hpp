#ifndef GET_HPP
# define GET_HPP

# include "Request.hpp"
# include "IHttpMethod.hpp"

class Response;

class Get : public IHttpMethod
{
	public:
		Get(Response &objectRef);
		~Get(void);

		void handleRequest(void);
	private:
		Response &response;

		void responseToFile(void);
		void responseToDirectory(void);
		void responseToInvalid(void);
		void responseCGI(void);
};

#endif // GET_HPP
