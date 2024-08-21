#ifndef PARSEBODYINFO_HPP
# define PARSEBODYINFO_HPP

# include <sys/stat.h>
# include <string.h>
# include <string>
# include <iostream>
# include "Config.hpp"
# include "Request.hpp"

class ParseBodyInfo
{
	public:

		static void parseBodyInfo(std::string requestText, RequestInfo &info);

	private:
		// construtor privado para impedir instânciação
		ParseBodyInfo();
};

void 		parseBodyValues(RequestInfo &info);
void 		extractBody(std::string requestText, RequestInfo &info);
std::string stringWithNewDivisor(std::string text, std::string newDivisor);
std::string trim(const std::string& str);

#endif // PARSEBODYINFO_HPP
