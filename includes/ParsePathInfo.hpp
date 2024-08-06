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
bool		isFile(const std::string& path);
bool 		isDirectory(const std::string& path);
e_pathType	identifyPathType(std::string& path, ServerConfig& serverConfig, RequestInfo &info);
Permission	getPermissions(std::string path);
#endif // PATHCHECKER_HPP
