#include "Get.hpp"

void Get::handle(int client_fd, RequestInfo &requestInfo, Response &response)
{
	if (requestInfo.pathType == File || requestInfo.pathType == URL)
		responseToFile(client_fd, requestInfo, response);
	else if (requestInfo.pathType == Directory)
		responseToDirectory(client_fd, requestInfo, response);
	else
		responseToInvalid(client_fd, requestInfo, response);
}

void Get::responseToFile(int client_fd, RequestInfo &requestInfo, Response &response)
{
	if (!requestInfo.permissions.read)
		response.setResponse(403, FORBIDDEN_ERROR);
	else if (!requestInfo.fullPath.empty())
		response.setResponse(200, requestInfo.fullPath);
	else
		response.setResponse(404, NOT_FOUND_ERROR);
	response.sendResponse(client_fd);
}

void Get::responseToDirectory(int client_fd, RequestInfo &requestInfo, Response &response)
{
	if (!endsWith(requestInfo.requestedRoute, "/"))
		response.setResponse(301, requestInfo.fullPath);
	else if (!requestInfo.fullPath.empty())
		response.setResponse(200, requestInfo.fullPath);
	else if (requestInfo.auto_index)
		// Return auto-index of directory
	else
		response.setResponse(403, FORBIDDEN_ERROR);
	response.sendResponse(client_fd);
}

void Get::responseToInvalid(int client_fd, RequestInfo &requestInfo, Response &response)
{
	response.setResponse(400, BAD_REQUEST_ERROR);
	response.sendResponse(client_fd);
}
