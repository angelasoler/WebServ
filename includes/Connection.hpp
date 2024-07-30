
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
# include <cerrno>
# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
class Connection
{
	private:
		Request						request;
		Response					response;
		std::map<int, std::string>	requestsText;
		size_t						nPolls;

		int		setNonBlocking(int client_fd);
		void	responseToClient(int client_fd);
		void	connectNewClient(Server &refServer);
		void	readClientRequest(int client_fd, int clientIdx);
		void	treatRequest(int client_fd, int clientIdx);
	public:
		std::vector<struct pollfd>	poll_fds;//leak

		Connection();
		Connection(const Connection &cpy);
		~Connection(void);
		bool	eventIO(void);
		void	verifyServerPollin(std::vector<Server> &servers);
		void	requestResponse(int nServers);
		void	cleanPollFds(void);
};


#endif /* CONNECTION_HPP */
