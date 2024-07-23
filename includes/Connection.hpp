
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
# include "Request.hpp"
# include "Response.hpp"

class	Server;

class Connection
{
	private:
		Request		request;
		Response	response;
		Server		&refServer;

		int		setNonBlocking(int client_fd);
	public:
		Connection(Server &server);
		Connection(const Connection &cpy);
		~Connection(void);

		void	responseToClient(int client_fd);
		void	newClient();
		void	readClientRequest(int client_fd, int clientIdx);
		void	breakRequest(int client_fd, int clientIdx);
};


#endif /* CONNECTION_HPP */
