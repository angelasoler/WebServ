#ifndef PATHCHECKER_HPP
# define PATHCHECKER_HPP

# include <sys/stat.h>
# include <string.h>
# include <string>
# include <iostream>
# include "Config.hpp"

class PathChecker
{
	public:
		enum PathType {
			File,
			Directory,
			URL,
			Missing,
			CGI
		};

		// método estático para verificar o tipo de rota
		static PathType checkPathType(const std::string& path, ServerConfig &serverConfig);

	private:
		// construtor privado para impedir instânciação
		PathChecker();

		static bool isFile(const std::string& path);
		static bool isDirectory(const std::string& path);
		static bool isUrlRouteinRouteConfig(const std::string& path, ServerConfig &serverConfig);
		static bool isCGI(const std::string& path);
};

// AUX function;
bool endsWith(const std::string& str, const std::string& suffix);

#endif // PATHCHECKER_HPP
