#include "Delete.hpp"

void Delete::handle(int client_fd, RequestInfo &requestInfo, Response &response)
{
	if (requestInfo.pathType == File)
	{
		// Handle file deletion
		response.setResponse(204, NO_CONTENT);
	}
	else if (!endsWith(requestInfo.requestedRoute, "/"))
		response.setResponse(409, CONFLICT_ERROR);
	else if (!requestInfo.permissions.write)
		response.setResponse(403, FORBIDDEN_ERROR);
	else if (deleteDirectory(requestInfo))
		response.setResponse(204, NO_CONTENT);
	else
		response.setResponse(500, INTERNAL_SERVER_ERROR);
	response.sendResponse(client_fd);
}

bool Delete::deleteDirectory(RequestInfo &requestInfo)
{
	// Try to delete a directory
	return true;
}
