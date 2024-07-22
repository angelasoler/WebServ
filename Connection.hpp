
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
# include "Request.hpp"
# include "Response.hpp"

class Connection
{
	private:
		Request		request;
		Response	response;
	public:
		Connection(void);
		~Connection(void);
};

#endif /* CONNECTION_HPP */
