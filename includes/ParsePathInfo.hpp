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
		static bool isFile(RequestInfo &info);
		static bool isDirectory(RequestInfo &info);
		static bool isUrlRouteinRouteConfig(RequestInfo &info);
		static bool isCGI(RequestInfo &info);
};

// AUX function;
bool endsWith(const std::string& strconst, const std::string &suffix);

#endif // PATHCHECKER_HPP
