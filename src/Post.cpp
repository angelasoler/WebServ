#include "Post.hpp"

void Post::handle(int client_fd, RequestInfo &requestInfo, Response &response)
{
	upload(client_fd, requestInfo, response);
}

void Post::upload(int client_fd, RequestInfo &requestInfo, Response &response)
{
	// Handle file upload logic here
}
