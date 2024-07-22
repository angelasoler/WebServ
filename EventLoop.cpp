
#include "EventLoop.hpp"

EventLoop::EventLoop(Server &server) : refServer(server) {}

EventLoop::~EventLoop(void) {}

// TO-DO
// [_] Make a state machine to handle each event
void	EventLoop::run()
{
	while (true)
	{
		int poll_count = poll(refServer.poll_fds.begin().base(), refServer.poll_fds.size(), -1);

		if (poll_count < 0)
			handleError("Poll failed");
		if (refServer.poll_fds[0].revents & POLLIN)
			refServer.connectNewClient();
		for (size_t i = 1; i < refServer.poll_fds.size(); i++)
		{
			if (refServer.poll_fds[i].revents & POLLIN)
				refServer.getRequest(refServer.poll_fds[i].fd, i);
			if (refServer.poll_fds[i].revents & POLLOUT && \
				!refServer.request.empty() && \
				!refServer.request[refServer.poll_fds[i].fd].empty())
			{
				refServer.treatRequest(refServer.poll_fds[i].fd, i);
			}
		}
	}
}
