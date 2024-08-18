#ifndef DELETE_HPP
# define DELETE_HPP

# include "Request.hpp"
# include "IHttpMethod.hpp"

class Response;

class Delete : public IHttpMethod
{
	public:
		Delete(Response &objectRef);
		~Delete(void);

		void handleRequest(void);
	private:
		Response &response;

		bool deleteDirectory(void);
};

#endif // DELETE_HPP
