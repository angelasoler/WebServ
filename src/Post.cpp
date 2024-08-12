#include "Post.hpp"
#include "Response.hpp"

void Post::handle(RequestInfo &requestInfo, Response &response)
{
	upload(requestInfo, response);
}

void Post::upload(RequestInfo &requestInfo, Response &response)
{
	(void)requestInfo;
	(void)response;
	// Handle file upload logic here
}
