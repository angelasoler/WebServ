#include "Get.hpp"
#include "Response.hpp"

void Get::handle(RequestInfo &requestInfo, Response &response)
{
	if (requestInfo.pathType == File || requestInfo.pathType == URL)
		responseToFile(requestInfo, response);
	else if (requestInfo.pathType == Directory)
		responseToDirectory(requestInfo, response);
	else
		responseToInvalid(requestInfo, response);
}

void Get::responseToFile(RequestInfo &requestInfo, Response &response)
{
	if (!requestInfo.permissions.read)
		response.setResponseMsg(403, FORBIDDEN_ERROR);
	else if (!requestInfo.fullPath.empty())
		response.setResponseMsg(200, requestInfo.fullPath);
	else
		response.setResponseMsg(404, NOT_FOUND_ERROR);
}

void Get::responseToDirectory(RequestInfo &requestInfo, Response &response)
{
	if (!endsWith(requestInfo.requestedRoute, "/"))
		response.setResponseMsg(301, requestInfo.fullPath);
	else if (!requestInfo.fullPath.empty())
		response.setResponseMsg(200, requestInfo.fullPath);
	else if (requestInfo.auto_index) {}
		// Return auto-index of directory
	else
		response.setResponseMsg(403, FORBIDDEN_ERROR);
}

void Get::responseToInvalid(RequestInfo &requestInfo, Response &response)
{
	(void)requestInfo;
	response.setResponseMsg(400, BAD_REQUEST_ERROR);
}
