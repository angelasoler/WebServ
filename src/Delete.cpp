#include "Delete.hpp"
#include "Response.hpp"

Delete::Delete(Response &objectRef) : response(objectRef) {}

Delete::~Delete(void) {}

void Delete::handleRequest(void)
{
	if (response.requestInfo.pathType == File)
	{
		// Handle file deletion
		response.setResponseMsg(204, NO_CONTENT);
	}
	else if (!endsWith(response.requestInfo.requestedRoute, "/"))
		response.setResponseMsg(409, CONFLICT_ERROR);
	else if (!response.requestInfo.permissions.write)
		response.setResponseMsg(403, FORBIDDEN_ERROR);
	else if (deleteDirectory())
		response.setResponseMsg(204, NO_CONTENT);
	else
		response.setResponseMsg(500, INTERNAL_SERVER_ERROR);
}

bool Delete::deleteDirectory(void)
{
	// Try to delete a directory
	return true;
}
