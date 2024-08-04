#include "ParsePathInfo.hpp"
#include <sys/stat.h>
#include <string.h>

void	ParsePathInfo::parsePathInfo(RequestInfo &info)
{
	if (parseAsUrl(info))
		info.pathType = URL;
	else if (parseAsCGI(info))
		info.pathType = CGI;
	else if (parseAsFile(info))
		info.pathType = File;
	else if (parseAsDirectory(info))
		info.pathType = Directory;
	info.pathType = URL;
}

bool ParsePathInfo::parseAsFile(RequestInfo &info)
{
	for (std::map<std::string, RouteConfig>::const_iterator it = info.serverConfig.routes.begin(); it != info.serverConfig.routes.end(); ++it)
	{
		std::string final_path = std::string(it->second.root_directory) + std::string(info.requestPath);
		struct stat buffer;
		if (stat(info.requestPath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode))
			return true;
	}
	return (false);
}

bool ParsePathInfo::parseAsDirectory(RequestInfo &info)
{
	for (std::map<std::string, RouteConfig>::const_iterator it = info.serverConfig.routes.begin(); it != info.serverConfig.routes.end(); ++it)
	{
		std::string final_path = std::string(it->second.root_directory) + std::string(info.requestPath);
		struct stat buffer;
		if (stat(final_path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
			return true;
	}
	return false;
}

bool ParsePathInfo::parseAsUrl(RequestInfo &info)
{
	(void)info;
	for (std::map<std::string, RouteConfig>::const_iterator it = info.serverConfig.routes.begin(); it != info.serverConfig.routes.end(); ++it)
	{
		if (it->second.path == info.requestPath)
			return true;
	}
	return false;
}

bool ParsePathInfo::parseAsCGI(RequestInfo &info)
{
	if (endsWith(info.requestPath, DEFAULT_CGI_EXTENSION))
		return true;
	return false;
}

bool endsWith(const std::string& str, const std::string& suffix) {
	if (suffix.size() > str.size()) {
		return false;
	}
	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}