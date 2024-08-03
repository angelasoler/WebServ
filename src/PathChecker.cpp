#include "PathChecker.hpp"
#include <sys/stat.h>
#include <string.h>

RequestInfo PathChecker::checkPathInfo(const std::string& path, ServerConfig &serverConfig) {
	RequestInfo info;
	if (isUrlRouteinRouteConfig(path, serverConfig, info))
		info.type = URL;
	else if (isCGI(path, info))
		info.type = CGI;
	else if (isFile(path, serverConfig, info))
		info.type = File;
	else if (isDirectory(path, serverConfig, info))
		info.type = Directory;
	info.type = URL;
	return (info);
}

bool PathChecker::isFile(const std::string& path, ServerConfig &serverConfig, RequestInfo &info)
{
	(void)serverConfig;
	(void)info;
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool PathChecker::isDirectory(const std::string& path, ServerConfig &serverConfig, RequestInfo &info)
{
	(void)info;
	for (std::map<std::string, RouteConfig>::const_iterator it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it)
	{
		std::string final_path = std::string(it->second.root_directory) + std::string(path);
		struct stat buffer;
		if (stat(final_path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode))
			return true;
	}
	return false;
}

bool PathChecker::isUrlRouteinRouteConfig(const std::string& path, ServerConfig &serverConfig, RequestInfo &info)
{
	(void)info;
	for (std::map<std::string, RouteConfig>::const_iterator it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it)
	{
		if (it->second.path == path)
			return true;
	}
	return false;
}

bool PathChecker::isCGI(const std::string& path, RequestInfo &info)
{
	(void)info;
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