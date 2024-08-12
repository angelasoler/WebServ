#include "Delete.hpp"
#include "Response.hpp"

void Delete::handle(int client_fd, RequestInfo &requestInfo, Response &response)
{
	if (requestInfo.pathType == File)
	{
		// Handle file deletion
		response.setResponseMsg(204, NO_CONTENT);
	}
	else if (!endsWith(requestInfo.requestedRoute, "/"))
		response.setResponseMsg(409, CONFLICT_ERROR);
	else if (!requestInfo.permissions.write)
		response.setResponseMsg(403, FORBIDDEN_ERROR);
	else if (deleteDirectory(requestInfo))
		response.setResponseMsg(204, NO_CONTENT);
	else
		response.setResponseMsg(500, INTERNAL_SERVER_ERROR);
	response.sendResponse(client_fd);
}

bool Delete::deleteDirectory(RequestInfo &requestInfo)
{
	(void)requestInfo;
	// Try to delete a directory
	return true;
}
