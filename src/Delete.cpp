#include "Delete.hpp"
#include "Response.hpp"

Delete::Delete(Response &objectRef) : response(objectRef) {}

Delete::~Delete(void) {}

void Delete::buildBody(void)
{
	std::string body = response.getDefaultPage();
	response.setBody(body);
}

int Delete::handleRequest(void)
{
	if (response.requestInfo.permissions.notFound)
		return 404;
	else if (response.requestInfo.pathType == File)
	{
		std::string parentDir = response.requestInfo.fullPath.substr(0, response.requestInfo.fullPath.find_last_of('/'));
		if (!hasWritePermission(parentDir))
			return 403;
		if (remove(response.requestInfo.fullPath.c_str()) == 0)
			return 204;
		else
			return 500;
	}
	else if (response.requestInfo.pathType == Directory)
	{
		if (deleteDirectory())
			return 204;
		else
			return 403;
	}
	return 500;
}

bool Delete::deleteDirectory(void)
{
	if (!hasWritePermission(response.requestInfo.fullPath))
		return false;
	DIR *dir = opendir(response.requestInfo.fullPath.c_str());
	if (!dir)
		return false;

	struct dirent *entry;
	while ((entry = readdir(dir)) != NULL)
	{
		std::string entryName = entry->d_name;

		if (entryName == "." || entryName == "..")
		{
			continue;
		}
		std::string fullPath = response.requestInfo.fullPath + "/" + entryName;
		struct stat entryStat;
		if (stat(fullPath.c_str(), &entryStat) == 0)
		{
			if (S_ISDIR(entryStat.st_mode))
			{
				Delete subDirDelete(response);
				response.requestInfo.fullPath = fullPath;
				if (!subDirDelete.deleteDirectory())
				{
					closedir(dir);
					return false;
				}
			}
			else
			{
				if (remove(fullPath.c_str()) != 0)
				{
					closedir(dir);
					return false;
				}
			}
		}
		else
		{
			closedir(dir);
			return false;
		}
	}

	closedir(dir);
	if (rmdir(response.requestInfo.fullPath.c_str()) != 0)
		return false;
	return true;
}
