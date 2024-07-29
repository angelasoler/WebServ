
# include "Request.hpp"
# include "Response.hpp"


Response::Response(void) {}

Response::~Response(void) {}

void	Response::sendResponse(int client_fd)
{
	send(client_fd, "Hello\n", 6, 0);
}

int	Response::treatActionAndResponse(std::map<int, std::string> request, int client_fd, e_httpMethodActions action)
{
	if (!request.empty() && !request[client_fd].empty())
	{
		switch (action)
		{
			case RESPONSE:
				sendResponse(client_fd);
				break;
			case CLOSE:
				request.erase(request.find(client_fd));
				break;
		}
		return (1);
	}
	return (0);
}

