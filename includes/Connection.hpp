
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
class Connection
{
	private:
		Request						request;
		Response					response;
		std::map<int, std::string>	requestsText;

		int			setNonBlocking(int client_fd);
	public:
		std::vector<struct pollfd>	poll_fds;//leak

		Connection();
		Connection(const Connection &cpy);
		~Connection(void);

		void	responseToClient(int client_fd);
		void	connectNewClient(Server &refServer);
		void	readClientRequest(int client_fd, int clientIdx);
		void	treatRequest(int client_fd, int clientIdx);
};


#endif /* CONNECTION_HPP */
