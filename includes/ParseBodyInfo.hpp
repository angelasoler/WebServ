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

#endif // PARSEBODYINFO_HPP
