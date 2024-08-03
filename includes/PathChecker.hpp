#ifndef PATHCHECKER_HPP
# define PATHCHECKER_HPP

# include <sys/stat.h>
# include <string.h>
# include <string>
# include <iostream>
# include "Config.hpp"
# include "Request.hpp"

class PathChecker
{
	public:

		// método estático para verificar o tipo de rota
		static RequestInfo checkPathInfo(const std::string& path, ServerConfig &serverConfig);

	private:
		// construtor privado para impedir instânciação
		PathChecker();
		static bool isFile(const std::string& path, ServerConfig &serverConfig, RequestInfo &info);
		static bool isDirectory(const std::string& path, ServerConfig &serverConfig, RequestInfo &info);
		static bool isUrlRouteinRouteConfig(const std::string& path, ServerConfig &serverConfig, RequestInfo &info);
		static bool isCGI(const std::string& path, RequestInfo &info);
};

// AUX function;
bool endsWith(const std::string& str, const std::string& suffix);

#endif // PATHCHECKER_HPP
