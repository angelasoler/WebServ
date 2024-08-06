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
		static bool parseAsFile(RequestInfo &info);
		static bool parseAsDirectory(RequestInfo &info);
		static bool parseAsUrl(RequestInfo &info);
		static bool parseAsCGI(RequestInfo &info);

		static e_pathType	getPathType(RequestInfo &info);
};

// AUX function;
bool endsWith(const std::string& strconst, const std::string &suffix);

#endif // PATHCHECKER_HPP
