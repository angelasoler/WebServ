#ifndef GET_HPP
# define GET_HPP

# include "Request.hpp"
class Response;

class Get
{
	public:
		void handle(RequestInfo &requestInfo, Response &response);
	private:
		void responseToFile(RequestInfo &requestInfo, Response &response);
		void responseToDirectory(RequestInfo &requestInfo, Response &response);
		void responseToInvalid(RequestInfo &requestInfo, Response &response);
};

#endif // GET_HPP
