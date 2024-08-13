#include "Get.hpp"
#include "Response.hpp"

void Get::handle(Response &response)
{
	if (response.requestInfo.pathType == File || response.requestInfo.pathType == URL)
		responseToFile(response);
	else if (response.requestInfo.pathType == Directory)
		responseToDirectory(response);
	else if (response.requestInfo.pathType == CGI)
		responseCGI(response);
	else
		responseToInvalid(response);
}

void Get::responseToFile(Response &response)
{
	if (!response.requestInfo.permissions.read)
		response.setResponseMsg(403, FORBIDDEN_ERROR);
	else if (!response.requestInfo.fullPath.empty())
		response.setResponseMsg(200, response.requestInfo.fullPath);
	else
		response.setResponseMsg(404, NOT_FOUND_ERROR);
}

void Get::responseToDirectory(Response &response)
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

void Get::responseToInvalid(Response &response)
{
	response.setResponseMsg(400, BAD_REQUEST_ERROR);
}

void Get::responseCGI(Response &response) {
	std::string	htmlResponse;
	CGIServer	cgi(response.requestInfo.requestedRoute);

	cgi.setEnv(response.requestInfo);
	htmlResponse = cgi.executeScript(response.requestInfo.body);
	response.setResponseMsg(200, htmlResponse);
}
