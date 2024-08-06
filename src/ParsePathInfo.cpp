#include "ParsePathInfo.hpp"
#include <sys/stat.h>
#include <string.h>



bool isFile(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
}

bool isDirectory(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

e_pathType identifyPathType(std::string& path, ServerConfig& serverConfig, RequestInfo &info)
{
	if (serverConfig.cgi.path_info == path || serverConfig.cgi.script_path == path) {
		return CGI;
	}

	// iterar pelas rotas configuradas
	std::map<std::string, RouteConfig>::const_iterator it;
	for (it = serverConfig.routes.begin(); it != serverConfig.routes.end(); ++it)
	{
		const RouteConfig& route = it->second;

		// Se o caminho corresponde à rota configurada
		if (path == route.path || path.find(route.path) == 0)
		{

			// Define o caminho completo pro caso de ser um diretoio ou arquivo
			info.fullPath = std::string(route.root_directory) + std::string(path);

			// Verificar se é um diretório
			if (isDirectory(info.fullPath))
				return Directory;

			// Verificar se é um arquivo
			else if (isFile(info.fullPath))
				return File;

			// Verificar se há uma redireção configurada
			else if (!route.redirection.empty())
				return Redirection;

			// Verificar se o caminho está associado a um CGI
			else if (endsWith(info.fullPath, DEFAULT_CGI_EXTENSION))
				return CGI;
		}
	}
	// Se não encontrar nenhuma correspondência específica, tratar como uma URL padrão
	return URL;
}

void	ParsePathInfo::parsePathInfo(RequestInfo &info)
{
	identifyPathType(info.path, info.serverRef, info);
}

bool endsWith(const std::string& str, const std::string& suffix) {
	if (suffix.size() > str.size()) {
		return false;
	}
	return std::equal(suffix.rbegin(), suffix.rend(), str.rbegin());
}