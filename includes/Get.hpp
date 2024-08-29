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

		int handleRequest(void);
	private:
		Response &response;

		int			responseToFile(void);
		int			responseToDirectory(void);
		int			responseToInvalid(void);
		int			responseCGI(void);
		int			responseToRedirection(void);
		std::string	getBodyFromFile(const std::string& fileName);
		void		buildBody(void);
};

#endif // GET_HPP
