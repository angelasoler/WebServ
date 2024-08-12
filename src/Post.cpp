#include "Post.hpp"
#include "Response.hpp"

void Post::handle(int client_fd, RequestInfo &requestInfo, Response &response)
{
	upload(client_fd, requestInfo, response);
}

void Post::upload(int client_fd, RequestInfo &requestInfo, Response &response)
{
	(void)client_fd;
	(void)requestInfo;
	(void)response;
	// Handle file upload logic here
}
