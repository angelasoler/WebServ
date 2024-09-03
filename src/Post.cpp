#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

int	Post::handleRequest(void)
{
	if (response.requestInfo.pathType == CGI)
		return responseCGI();
	// return upload();
	return (404);
}

void	Post::upload(void)
{
	// Handle file upload logic here
}

void	Post::buildBody(void) {}

int	Post::responseCGI(void) {
	CGIServer	cgi(response.requestInfo);

	cgi.setEnv();
	cgi.executeScript();

	response.setBody(cgi.CGIReturn.body);
	return(cgi.CGIReturn.code);
}
