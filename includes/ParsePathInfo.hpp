#ifndef PARSEPATHINFO_HPP
# define PARSEPATHINFO_HPP

# include <sys/stat.h>
# include <string.h>
# include <string>
# include <iostream>
# include "Config.hpp"
# include "Request.hpp"

class ParsePathInfo
{
	public:

		// método estático para verificar o tipo de rota
		static void parsePathInfo(RequestInfo &info);

	private:
		// construtor privado para impedir instânciação
		ParsePathInfo();
};

// AUX function;
bool		endsWith(const std::string& strconst, const std::string &suffix);
bool		startsWith(const std::string& str, const std::string& prefix);
std::string composeFullPath(const std::string &prefix, const std::string &sufix);
bool		isFile(const std::string& path);
bool 		isDirectory(const std::string& path);
e_pathType	identifyFullPathType(std::string& requestedRoute, ServerConfig& serverConfig, RequestInfo &info);
Permission	getPermissions(std::string path);
#endif // PARSEPATHINFO_HPP
