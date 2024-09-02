#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

int	Post::handleRequest(void)
{
	// std::cout << response.requestInfo.body << "\n";
	// std::cout << response.getClientFd() << "\n";
	(void)response;
	if (response.requestInfo.pathType == CGI)
		responseCGI();
	return (404);
}

void	Post::upload(void)
{
	// Handle file upload logic here
}

void	Post::buildBody(void) {}

int	Post::responseCGI(void) {
	htmlResponse	htmlResponse;
	CGIServer		cgi(response.requestInfo.fullPath);

	cgi.setEnv(response.requestInfo);
	htmlResponse = cgi.executeScript(response.requestInfo.body);
	response.setBody(htmlResponse.body);
	return(htmlResponse.code);
}
