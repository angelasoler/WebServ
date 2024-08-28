#include "ParsePathInfo.hpp"
#include <cerrno>
#include <sys/stat.h>
#include <string.h>


void	ParsePathInfo::parsePathInfo(RequestInfo &info)
{
	info.fullPath = identifyFullPath(info);
	info.pathType = identifyType(info);
	info.permissions = getPermissions(info.fullPath);
}

std::string	identifyFullPath(RequestInfo &info)
{
	RouteConfig routeConfig;
	std::string	fullPath;

	std::map<std::string, RouteConfig>::iterator it;
	for (it = info.serverRef.routes.begin(); it != info.serverRef.routes.end(); ++it)
	{
		routeConfig = it->second;
		info.auto_index = routeConfig.directory_listing;

		// Se o caminho corresponde exatamente à rota configurada
		if ((info.requestedRoute == routeConfig.route))
		{
			//DAQUI SAÍ UM FILE
			fullPath = composeFullPath(routeConfig.root_directory, routeConfig.default_file);
			break ;
		}
		// se o caminho é composto por uma rota
		if (startsWith(info.requestedRoute, routeConfig.route))
		{
			//DAQUI SAI O ROOT DIRECTORY, CONCATENANDO COM O QUE SOBRAR DO requestRoute sem a rota
			std::string requestSuffix = info.requestedRoute.substr(routeConfig.route.size());
			fullPath = composeFullPath(routeConfig.root_directory, requestSuffix);
			info.fullPath = composeFullPath(info.fullPath, "index.html");
			break ;
		}
		else
			// Quando é a rota / + <arquivo>, ou não achou a rota
			fullPath = composeFullPath(routeConfig.root_directory, info.requestedRoute);
	}
	// if (isDirectory(info.fullPath))
	info.configRef = routeConfig;
	return fullPath;
}

e_pathType identifyType(RequestInfo &info)
{
	if (info.requestedRoute == "/" && \
		info.serverRef.routes.find("/") == info.serverRef.routes.end())
		return UNKNOWN;
	if (!info.configRef.redirection.empty())
		return Redirection;
	if (endsWith(info.fullPath, DEFAULT_CGI_EXTENSION))
		return CGI;
	if (isFile(info.fullPath))
		return File;
	if (isDirectory(info.fullPath)) {
		info.fullPath = composeFullPath(info.fullPath, info.configRef.default_file);
		struct stat buffer;
		if (!(stat(info.fullPath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode)) || info.configRef.default_file.empty())
			info.fullPath.clear();
		return Directory;
	}
	return UNKNOWN;
}

std::string composeFullPath(const std::string& prefix, const std::string& suffix)
{
	std::string fullPath = prefix;

	// Remove '/' extra do sufixo se o prefixo já terminar com '/'
	if (endsWith(prefix, "/")) {
		// Remove '/' do início do sufixo, se houver
		if (!suffix.empty() && suffix[0] == '/')
			fullPath += suffix.substr(1); // Adiciona o sufixo sem o primeiro '/'
		else
			fullPath += suffix;
	}
	else {
		fullPath += "/" + suffix;
	}
	return fullPath;
}

bool isFile(const std::string& path) {
	struct stat buffer;

	if (access(path.c_str(), R_OK)) {
		switch (errno)
		{
			case EACCES:
				return true;
			
			case ENOENT:
				return false;
		}
	}
	return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool isDirectory(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

bool endsWith(const std::string& str, const std::string& suffix) {
	if (suffix.size() > str.size()) {
		return false;
	}
	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}

bool startsWith(const std::string& str, const std::string& prefix) {
	if (prefix == "/")
		return false;
	if (prefix.size() > str.size()) {
		return false;
	}
	return std::equal(prefix.begin(), prefix.end(), str.begin());
}

Permission getPermissions(std::string path)
{
	Permission permissions;

	if (access(path.c_str(), R_OK)) {
		if (errno == ENOENT) {
			permissions.notFound = true;
			permissions.read = -1;
			permissions.write = -1;
			permissions.execute = -1;
			return permissions;
		}
	}
	permissions.read = (access(path.c_str(), R_OK) == 0);
	permissions.write = (access(path.c_str(), W_OK) == 0);
	permissions.execute = (access(path.c_str(), X_OK) == 0);
	permissions.notFound = false;
	return permissions;
}
