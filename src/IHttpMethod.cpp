# include "IHttpMethod.hpp"
# include "Response.hpp"
# include "CGIServer.hpp"

IHttpMethod::~IHttpMethod() {}

int	IHttpMethod::responseCGI(Response &response) {
	CGIServer	cgi(response.requestInfo);

	cgi.setEnv();
	cgi.executeScript();

	response.setBody(cgi.CGIReturn.body);
	return(cgi.CGIReturn.code);
}

bool IHttpMethod::hasWritePermission(const std::string &path)
{
	struct stat pathStat;
	if (stat(path.c_str(), &pathStat) != 0)
		return false;

	return (pathStat.st_mode & S_IWUSR) || (pathStat.st_mode & S_IWGRP) || (pathStat.st_mode & S_IWOTH);
}

bool IHttpMethod::fileExists(const std::string& filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

bool IHttpMethod::dirExists(const std::string& dirname)
{
	struct stat buffer;
	return (stat(dirname.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}

