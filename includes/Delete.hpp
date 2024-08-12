#ifndef DELETE_HPP
# define DELETE_HPP

# include "Request.hpp"
# include "Response.hpp"

class Delete
{
	public:
		void handle(int client_fd, RequestInfo &requestInfo, Response &response);
	private:
		bool deleteDirectory(RequestInfo &requestInfo);
};

#endif // DELETE_HPP
