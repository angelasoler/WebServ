#ifndef GET_HPP
# define GET_HPP

# include "Request.hpp"
class Response;

class Get
{
	public:
		void handle(Response &response);
	private:
		void responseToFile(Response &response);
		void responseToDirectory(Response &response);
		void responseToInvalid(Response &response);
		void responseCGI(Response &response);
};

#endif // GET_HPP
