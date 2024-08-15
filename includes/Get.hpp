#ifndef GET_HPP
# define GET_HPP

# include "Request.hpp"
class Response;

class Get
{
	public:
		Get(Response &objectRef);

		void handler(void);
	private:
		Response &response;

		void responseToFile(void);
		void responseToDirectory(void);
		void responseToInvalid(void);
		void responseCGI(void);
};

#endif // GET_HPP
