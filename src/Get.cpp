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
	if (response.requestInfo.configRef.directory_listing)
	{
		if (*response.requestInfo.fullPath.rbegin() != '/')
		{
			std::stringstream	port;

			port << response.requestInfo.serverRef.port;
			response.setHeader("Location", "http://localhost:" + port.str() + response.requestInfo.requestedRoute + "/");
			return 307;
		}
		response.setBody(makeBodyForDirListing());
		if (response.getBody().empty())
			return (500);
		return 200;
	}
	return 404;
}

int	Get::responseToInvalid(void)
{
	return(404);
}

int	Get::responseToRedirection(void)
{
	response.setHeader("Location", response.requestInfo.configRef.redirection);
	return(307);
}

int	Get::responseCGI(void) {
	htmlResponse	htmlResponse;
	CGIServer		cgi(response.requestInfo.fullPath);

	cgi.setEnv(response.requestInfo);
	htmlResponse = cgi.executeScript(response.requestInfo.body);
	response.setBody(htmlResponse.body);
	return(htmlResponse.code);
}
