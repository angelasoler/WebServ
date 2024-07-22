
#include "EventLoop.hpp"

EventLoop::EventLoop(Server &server) : refServer(server) {}

EventLoop::~EventLoop(void) {}

// TO-DO
// [_] Make a state machine to handle each event
void	EventLoop::run()
{
	size_t	nPolls;

	while (true)
	{
		nPolls = refServer.poll_fds.size();
		if (poll(refServer.poll_fds.begin().base(), nPolls, -1) < 0) {
			handleError("Poll failed");
		}
		if (refServer.poll_fds[0].revents & POLLIN) {
			refServer.connectNewClient();
		}
		for (size_t clientIdx = N_SERVERS; clientIdx < nPolls; clientIdx++)
		{
			if (refServer.poll_fds[clientIdx].revents & POLLIN) {
				refServer.getRequest(refServer.poll_fds[clientIdx].fd, clientIdx);
			}
			if (refServer.poll_fds[clientIdx].revents & POLLOUT) {
				refServer.treatRequest(refServer.poll_fds[clientIdx].fd, clientIdx);
			}
		}
	}
}
