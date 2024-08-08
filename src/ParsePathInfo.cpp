#include "ParsePathInfo.hpp"
#include <sys/stat.h>
#include <string.h>


void	ParsePathInfo::parsePathInfo(RequestInfo &info)
{
	info.pathType = identifyFullPathType(info.requestedRoute, info.serverRef, info);
	info.permissions = getPermissions(info.fullPath);
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
			info.fullPath = std::string(routeConfig.root_directory) + "/" + std::string(routeConfig.default_file);
			return File;
		}
		if (startsWith(requestedRoute, routeConfig.route))
		{
			std::string requestSuffix = requestedRoute.substr(routeConfig.route.size());
			info.fullPath = std::string(routeConfig.root_directory) + "/" +std::string(requestSuffix);

			// Verificar se é um diretório
			if (isDirectory(info.fullPath))
				return Directory;

			// Verificar se é um arquivo
			else if (isFile(info.fullPath))
				return File;

			// Verificar se há uma redireção configurada
			else if (!routeConfig.redirection.empty())
				return Redirection;

			// Verificar se o caminho está associado a um CGI
			else if (endsWith(info.fullPath, DEFAULT_CGI_EXTENSION))
				return CGI;

			// else if (info.serverRef.cgi.script_path == requestSuffix) // Caso a requestRoute for o script_path, tera default CGI ?
			// {
			// 	info.fullPath = std::string(routeConfig.root_directory) + "/" +std::string(requestSuffix);
			// }
		}
	}
	// Se não encontrar nenhuma correspondência específica, tratar como uma URL padrão
	return URL;
}

bool isFile(const std::string& path) {
	struct stat buffer;
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

	permissions.read = (access(path.c_str(), R_OK) == 0);
	permissions.write = (access(path.c_str(), W_OK) == 0);
	permissions.execute = (access(path.c_str(), X_OK) == 0);
	return permissions;
}
