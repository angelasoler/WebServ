#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

int	Post::handleRequest(void)
{
	if (response.requestInfo.pathType == CGI)
		return responseCGI(response);
	return (404);
}

void	Post::upload(void)
{
	// Handle file upload logic here
}

void	Post::buildBody(void) {}
