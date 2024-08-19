#include "Get.hpp"
#include "Response.hpp"

Get::Get(Response &objectRef) : response(objectRef) {}

Get::~Get(void) {}

void Get::handleRequest(void)
{
	if (response.requestInfo.pathType == File || response.requestInfo.pathType == URL)
		responseToFile();
	else if (response.requestInfo.pathType == Directory)
		responseToDirectory();
	else if (response.requestInfo.pathType == CGI)
		responseCGI();
	else
		responseToInvalid();
}

void Get::responseToFile(void)
{
	if (!response.requestInfo.permissions.read)
		return(403);
	else if (!response.requestInfo.fullPath.empty())
		response.setResponseMsg(200, response.requestInfo.fullPath);
	else
		response.setResponseMsg(404, NOT_FOUND_ERROR);
}

void Get::responseToDirectory(void)
{
	if (!endsWith(response.requestInfo.requestedRoute, "/"))
		response.setResponseMsg(301, response.requestInfo.fullPath);
	else if (!response.requestInfo.fullPath.empty())
		response.setResponseMsg(200, response.requestInfo.fullPath);
	else if (response.requestInfo.auto_index) {}
		// Return auto-index of directory
	else
		response.setResponseMsg(403, FORBIDDEN_ERROR);
}

void Get::responseToInvalid(void)
{
	response.setResponseMsg(400, BAD_REQUEST_ERROR);
}

void Get::responseCGI(void) {
	std::string	htmlResponse;
	CGIServer	cgi(response.requestInfo.requestedRoute);

	cgi.setEnv(response.requestInfo);
	htmlResponse = cgi.executeScript(response.requestInfo.body);
	response.setResponseMsg(200, htmlResponse);
}
