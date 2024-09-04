#ifndef IHTTPMETHOD_HPP
#define IHTTPMETHOD_HPP

# include "Request.hpp"
# include <string>
# include <fstream>
# include <sstream>
# include <sys/stat.h>

class Response;

class IHttpMethod {
	public:
		virtual void buildBody(void) = 0;
		virtual int handleRequest() = 0;
		virtual ~IHttpMethod();
		int		responseCGI(Response &response);
		bool		hasWritePermission(const std::string &path);
		bool		fileExists(const std::string& filename);
		bool		dirExists(const std::string& filename);
};

#endif
