#include "PathChecker.hpp"
#include <sys/stat.h>
#include <string.h>

PathChecker::PathType PathChecker::checkPathType(const std::string& path, ServerConfig &serverConfig) {
	if (isUrlRouteinRouteConfig(path, serverConfig))
		return URL;
	else if (isCGI(path))
		return CGI;
	else if (isFile(path))
		return File;
	else if (isDirectory(path))
		return Directory;
	return URL;
}

bool PathChecker::isFile(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool PathChecker::isDirectory(const std::string& path)
{
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool PathChecker::isUrlRouteinRouteConfig(const std::string& path, ServerConfig &serverConfig)
{
	for (std::map<std::string, RouteConfig>::const_iterator it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it)
	{
		if (it->second.path == path)
			return true;
	}
	return false;
}

bool PathChecker::isCGI(const std::string& path)
{
	if (endsWith(path, DEFAULT_CGI_EXTENSION))
		return true;
	return false;
}

bool endsWith(const std::string& str, const std::string& suffix) {
	if (suffix.size() > str.size()) {
		return false;
	}

	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}