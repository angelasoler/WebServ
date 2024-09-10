
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
		std::map<int, Request>			request;
		std::map<int, Response>			response;
		std::vector<Server>			servers;

		int		setNonBlocking(int client_fd);
		void	responseToClient(int client_fd);
		void	connectNewClient(Server &refServer);
		void	readClientRequest(int client_fd);
		void	treatRequest(int client_fd);
		void	initSockets(void);
		void	cleanClient(int clientIdx);
	public:
		size_t						nServers;
		std::vector<struct pollfd>	poll_fds;
		std::map<int, ServerConfig>	clientServerConfig;

		Connection();
		~Connection(void);

		bool	eventIO(void);
		void	verifyServerPollin(void);
		void	requestResponse(void);
		void	cleanPollFds(void);
};


#endif /* CONNECTION_HPP */
