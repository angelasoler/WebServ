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
			return responseCGI(response);;
		case File:
			return responseToFile();
		case Redirection:
			return responseToRedirection();
		default:
			return responseToInvalid();
	}
	return 0;
}

std::string	getBodyFromFile(const std::string& fileName)
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
	if (!response.getBody().empty())
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
	if (response.requestInfo.routeRef.directory_listing)
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
	if (response.requestInfo.requestedRoute == "Bad")
		return (400);
	if (response.requestInfo.requestedRoute.empty())
		return (405);
	return(404);

}

int	Get::responseToRedirection(void)
{
	response.setHeader("Location", response.requestInfo.routeRef.redirection);
	return(307);
}

std::string formatSize(off_t item_size)
{
	std::stringstream	size;
	const char			*unity[] = {"B", "KB", "MB", "GB"};
	const int			division[] = {1, 1024, 1024 * 1024, 1024 * 1024 * 1024};
	int					pos = 4;

	while (item_size < division[pos] && pos > 0)
		pos--;
	size << item_size / division[pos];
	size << " ";
	size << unity[pos];
	return size.str();
}

std::string formatTime(time_t item_time)
{
	struct tm *timeinfo;
	char buffer[20];

	timeinfo = localtime(&item_time);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M", timeinfo);
	return std::string(buffer);
}

std::string	formatHref(std::string &route, char *nameFile, std::string &path)
{
	std::string	href;

	if (*route.rbegin() == '/' || nameFile[0] == '/')
		href = route + nameFile;
	else
		href = route + "/" + nameFile;
	// (response.requestInfo.fullPath + nameFile)
	if (isDirectory(path + nameFile))
		href += "/";
	return href;
}

std::string Get::makeBodyForDirListing()
{
	DIR *listingDir = opendir(response.requestInfo.fullPath.c_str());

	if (listingDir == NULL)
		return ("");

	std::string body;
	std::string line_feed = "\r\n";

	body = "<html>" + line_feed;

	body += "<header>" + line_feed;
	body += "<title>" + line_feed;
	body += "Directory Listing of " + response.requestInfo.requestedRoute + line_feed;
	body += "</title>" + line_feed;
	body += "<style>" + line_feed;
	body += "th {padding-inline: 2rem}" + line_feed;
	body += "</style>" + line_feed;
	body += "</header>" + line_feed;

	body += "<body>" + line_feed;
	body += "<h1> Index of " + response.requestInfo.requestedRoute + "</h1>" + line_feed;
	body += "<table>" + line_feed;
	body += "<tr><th>Name</th><th>Last Modified</th><th>Size</th></tr>" + line_feed;

	struct dirent *item;
	struct stat item_status;

	while ((item = readdir(listingDir)) != NULL)
	{

		if (item->d_name[0] == '.' && !item->d_name[1])
			continue;
		if (stat((response.requestInfo.fullPath + item->d_name).c_str(), &item_status) == -1)
			return ("");

		// Valida se tem permissão de leitura
		if (!response.requestInfo.permissions.read)
			continue;
	
		// Monta a linha com as 3 colunas
		body += "<tr>";
		body += "<th>";

		body += "<a href=" + formatHref(response.requestInfo.requestedRoute, item->d_name, response.requestInfo.fullPath);		
		if (!isDirectory(response.requestInfo.fullPath + item->d_name))
			body += " style=\"color: green;\"";
		body += ">";
		
		body += item->d_name;
		if (isDirectory(response.requestInfo.fullPath + item->d_name))
			body += "/";
	
		body += "</a>";
		body += "</th>";

		if (std::strcmp(item->d_name, "..") != 0)
			body += "<th>" + formatTime(item_status.st_mtime) + "</th>";
		else
			body += "<th></th>";

		if (!isDirectory(response.requestInfo.fullPath + item->d_name))
			body += "<th>" + formatSize(item_status.st_size) + "</th>";
		else
			body += "<th>-</th>";
		
		body += "</tr>" + line_feed;
	}

	closedir(listingDir);

	body += "</table>" + line_feed;
	body += "</body>" + line_feed;

	body += "</html>" + line_feed;
	return body;
}
