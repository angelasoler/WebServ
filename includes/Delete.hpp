#ifndef DELETE_HPP
# define DELETE_HPP

# include "Request.hpp"
class Response;

class Delete
{
	public:
		void handle(Response &response);
	private:
		bool deleteDirectory(RequestInfo &requestInfo);
};

#endif // DELETE_HPP
