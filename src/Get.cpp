#include "Get.hpp"
#include "Response.hpp"

Get::Get(Response &objectRef) : response(objectRef) {}

Get::~Get(void) {}

int Get::handleRequest(void)
{
	switch (response.requestInfo.pathType)
	{
		case Directory:
			return responseToDirectory();
		case CGI:
			return responseCGI();
		case File:
			return responseToFile();
		case Redirection:
			return responseToRedirection();
		default:
			return responseToInvalid();
	}
}

std::string	Get::getBodyFromFile(const std::string& fileName)
{
	std::string ret;

	std::ifstream file(fileName.c_str());
	if (!file)
		return NOT_FOUND_ERROR;
	std::ostringstream oss;
	oss << file.rdbuf();
	ret = oss.str();
	file.close();
	return ret;
}

void	Get::buildBody(void)
{
	if (response.requestInfo.pathType == CGI)
		return ;
	std::string body = response.getDefaultPage();

	if (body == NO_DEFAULT_ERROR)
		body = getBodyFromFile(response.requestInfo.fullPath);
	response.setBody(body);
}

int	Get::responseToRedirection(void)
{
	std::stringstream port;

	port << response.requestInfo.serverRef.port;
	response.setHeader("Location", (std::string)"http://localhost:" + port.str() + (response.requestInfo.fullPath.at(0) == '.' ? response.requestInfo.fullPath.substr(1) : response.requestInfo.fullPath) );
	return (307);
}

int	Get::responseToFile(void)
{
	if (response.requestInfo.permissions.notFound)
		return (404);
	else if (!response.requestInfo.permissions.read)
		return (403);
	else if (!response.requestInfo.fullPath.empty())
		return (200);
	return(400);
}

int	Get::responseToDirectory(void)
{
	if (response.requestInfo.fullPath.empty()) {
		if (!response.requestInfo.configRef.directory_listing)
			return 404;
		else {
			response.requestInfo.fullPath = \
			response.requestInfo.configRef.root_directory + \
			"/dirListingPlaceHolder.html";
			// makeBodyForDirListing
			return (200);
		}
	}
	return (200);
}

int	Get::responseToInvalid(void)
{
	return(400);
}

int	Get::responseCGI(void) {
	std::string	htmlResponse;
	CGIServer	cgi(response.requestInfo.requestedRoute);

	cgi.setEnv(response.requestInfo);
	htmlResponse = cgi.executeScript(response.requestInfo.body);
	response.setBody(htmlResponse);
	return(200);
}
