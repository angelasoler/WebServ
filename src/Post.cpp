#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

int	Post::handleRequest(void)
{
	add_post_log("\n\n-----Starting Upload-----\n");
	// verifica se é cgi
	if (response.requestInfo.pathType == CGI) {
		add_post_log("Handling CGI request.");
		// handleCGI
	}

	if (isValidRoute()) {
		add_post_log("Valid route detected, proceeding with upload.");
		return upload();
	}

	add_post_log("Invalid route: " + response.requestInfo.fullPath);
	return (400);
}

int Post::upload(void)
{
	RequestInfo &info = response.requestInfo;

	if (info.contentType.find("multipart/form-data") != std::string::npos)
	{
		if (info.multipartHeaders.size() < 1 || info.multipartValues.size() < 1) {
			add_post_log("Multipart data is incomplete.");
			return 400;
		}

		for (size_t i = 0; i < info.multipartValues.size(); ++i)
		{
			std::string filename = getFileName(i);
			if (!writeFile(info.multipartValues[i], filename)) {
				add_post_log("Failed to write file: " + filename);
				return 400;
			}
			add_post_log("File uploaded successfully: " + filename);
		}
		return 201;
	}

	add_post_log("Unsupported content type: " + info.contentType);
	return 400;
}

bool	Post::isValidRoute(void) {
	RequestInfo &info = response.requestInfo;

	std::string uploadPath = info.configRef.root_directory + "/" + info.configRef.upload_directory + "/";
	if (response.requestInfo.pathType == Directory)
	{
		if (response.requestInfo.fullPath == uploadPath) {
			add_post_log("Route is valid for upload: " + uploadPath);
			return true;
		}
		add_post_log("Route is not valid for upload: " + response.requestInfo.fullPath);
		return false;
	}
	add_post_log("Path type is not a Directory.");
	return false;
}

bool	writeFile(const std::string& content, const std::string& fileName) {
	std::ofstream file(fileName.c_str());
	if (file.is_open()) {
		file << content;
		file.close();
		add_post_log("File written successfully: " + fileName);
		return true;
	} else {
		add_post_log("Error opening file: " + fileName);
	}
	return false;
}

void	Post::buildBody(void) {
	std::string body = response.getDefaultPage();

	if (body == NO_DEFAULT_ERROR)
		response.setBody(CREATED_SUCCESSFULLY);
	response.setBody(body);
}

bool Post::fileExists(const std::string& filename)
{
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

std::string	Post::getFileName(int index)
{
	RequestInfo	&info = response.requestInfo;
	std::string filename;
	std::string header = info.multipartHeaders[index];
	size_t pos = header.find("filename=");

	if (pos != std::string::npos) {
		size_t start = header.find("\"", pos + 5) + 1;
		size_t end = header.find("\"", start);
		filename = info.fullPath + header.substr(start, end - start);
		return filename;
	}
	return info.fullPath + "new_file";
}

void add_post_log(const std::string& content) {
	std::ofstream	logFd("logs/uploads.log", std::ios_base::app);
	logFd << content << std::endl;
}
