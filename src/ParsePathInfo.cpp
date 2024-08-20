#include "ParsePathInfo.hpp"
#include <cerrno>
#include <sys/stat.h>
#include <string.h>


void	ParsePathInfo::parsePathInfo(RequestInfo &info)
{
	std::cout << "requestedRoute: " << info.requestedRoute << "\n";
	info.pathType = identifyFullPathType(info.requestedRoute, info.serverRef, info);
	info.permissions = getPermissions(info.fullPath);
	// std::cout << "fullpath: " << info.fullPath << "\n";
}

e_pathType identifyFullPathType(std::string& requestedRoute, ServerConfig& serverConfig, RequestInfo &info)
{
	// iterar pelas rotas configuradas
	std::map<std::string, RouteConfig>::const_iterator it;
	for (it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it)
	{
		const RouteConfig& routeConfig = it->second;

		// Se o caminho corresponde à rota configurada
		if ((requestedRoute == routeConfig.route))
		{
			info.fullPath = composeFullPath(routeConfig.root_directory, routeConfig.default_file);
			return URL;
		}

		// Verifica se inicia buscando uma rota
		if (startsWith(requestedRoute, routeConfig.route))
		{
			std::string requestSuffix = requestedRoute.substr(routeConfig.route.size());
			info.fullPath = composeFullPath(routeConfig.root_directory, requestSuffix);
		}
		else
			info.fullPath = composeFullPath(routeConfig.root_directory, info.requestedRoute);

		// Verificar se o caminho está associado a um CGI
		if (endsWith(info.fullPath, DEFAULT_CGI_EXTENSION))
			return CGI;

		// Verificar se é um diretório
		else if (isDirectory(info.fullPath)) {
			// Se for um diretório, tentar encontrar o arquivo index dele
			info.fullPath = composeFullPath(info.fullPath, routeConfig.default_file);
			struct stat buffer;
			if (!(stat(info.fullPath.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode)) || routeConfig.default_file.empty())
				info.fullPath.clear();

			// Recolher auto-index
			info.auto_index = routeConfig.directory_listing;
			return Directory;
		}

		// Verificar se é um arquivo
		else if (isFile(info.fullPath)) {
			return File;
		}

		// Verificar se há uma redireção configurada
		else if (!routeConfig.redirection.empty())
			return Redirection;
	}
	// Se não encontrar nenhuma correspondência específica, tratar como UNKNOWN requests 
	info.fullPath.clear();
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
				std::cout << "no permission" << std::endl;
				return true;
			
			case ENOENT:
				std::cout << "not found" << std::endl;
				return true;
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
