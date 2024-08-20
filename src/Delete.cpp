#include "Delete.hpp"
#include "Response.hpp"

Delete::Delete(Response &objectRef) : response(objectRef) {}

Delete::~Delete(void) {}

void	Delete::buildBody(void)
{
	std::string body = response.getDefaultPage();

	response.setBody(body);
}

int Delete::handleRequest(void)
{
	if (response.requestInfo.pathType == File)
	{
		// Handle file deletion
		return (204);
	}
	else if (!endsWith(response.requestInfo.requestedRoute, "/"))
		return (409);
	else if (!response.requestInfo.permissions.write)
		return (403);
	else if (deleteDirectory())
		return (204);
	else
		return (500);
}

bool Delete::deleteDirectory(void)
{
	// Try to delete a directory
	return true;
}
