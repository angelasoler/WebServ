#include "Post.hpp"
#include "Response.hpp"

void Post::handle(Response &response)
{
	upload(response);
}

void Post::upload(Response &response)
{
	(void)response;
	// Handle file upload logic here
}
