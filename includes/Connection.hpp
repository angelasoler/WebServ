
#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
# include <cerrno>
# include "Request.hpp"
# include "Response.hpp"
# include "Server.hpp"
# include "Config.hpp"
class Connection
{
	private:
		Request						request;
		std::vector<Server>			servers;
		size_t						nPolls;

		int		setNonBlocking(int client_fd);
		void	responseToClient(int client_fd);
		void	connectNewClient(Server &refServer);
		void	readClientRequest(int client_fd);
		void	treatRequest(int client_fd);
		void	initSockets(void);
	public:
		size_t						nServers;
		std::vector<struct pollfd>	poll_fds;
		std::map<int, ServerConfig>	clientServerConfig;

		Connection();
		Connection(const Connection &cpy);
		~Connection(void);

		bool	eventIO(void);
		void	verifyServerPollin(void);
		void	requestResponse(void);
		void	cleanPollFds(void);
};


#endif /* CONNECTION_HPP */
