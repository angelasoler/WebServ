#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

int	Post::handleRequest(void)
{
	uploadLogs("\n\n-----Starting Upload-----\n");
	// verifica se é cgi
	if (response.requestInfo.pathType == CGI) {
		uploadLogs("Handling CGI request.");
		// handleCGI
	}

	if (isValidRoute()) {
		uploadLogs("Valid route detected, proceeding with upload.");
		return upload();
	}

	uploadLogs("Invalid route: " + response.requestInfo.fullPath);
	return (400);
}

bool	Post::isValidRoute(void) {
	RequestInfo &info = response.requestInfo;

	std::string uploadPath = info.configRef.root_directory + "/" + info.configRef.upload_directory + "/";
	if (response.requestInfo.pathType == Directory)
	{
		if (response.requestInfo.fullPath == uploadPath) {
			uploadLogs("Route is valid for upload: " + uploadPath);
			return true;
		}
		uploadLogs("Route is not valid for upload: " + response.requestInfo.fullPath);
		return false;
	}
	uploadLogs("Path type is not a Directory.");
	return false;
}

bool	writeFile(const std::string& content, const std::string& fileName) {
	std::ofstream file(fileName.c_str());
	if (file.is_open()) {
		file << content;
		file.close();
		uploadLogs("File written successfully: " + fileName);
		return true;
	} else {
		uploadLogs("Error opening file: " + fileName);
	}
	return false;
}

int Post::upload(void)
{
	RequestInfo &info = response.requestInfo;

	if (info.contentType.find("multipart/form-data") != std::string::npos)
	{
		if (info.multipartHeaders.size() < 1 || info.multipartValues.size() < 1) {
			uploadLogs("Multipart data is incomplete.");
			return 400;
		}

		for (size_t i = 0; i < info.multipartValues.size(); ++i)
		{
			std::string filename = getFileName();
			if (!writeFile(info.multipartValues[i], filename)) {
				uploadLogs("Failed to write file: " + filename);
				return 400;
			}
			uploadLogs("File uploaded successfully: " + filename);
		}
		return 201;
	}

	uploadLogs("Unsupported content type: " + info.contentType);
	return 400;
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

std::string	Post::getFileName(void)
{
	RequestInfo	&info = response.requestInfo;
	std::string filename;
	std::string header = info.multipartHeaders[0];
	size_t pos = header.find("filename=");

	if (pos != std::string::npos) {
		size_t start = header.find("\"", pos + 5) + 1;
		size_t end = header.find("\"", start);
		filename = info.fullPath + header.substr(start, end - start);
		// return filename;
	}
	// Se o filename foi encontrado
	if (filename.empty()) {
		filename = info.fullPath + "new_file";
	}
	// Verifica se o arquivo já existe
	if (fileExists(filename)) {
		int counter = 1;
		std::string newFilename;
		do {
			std::stringstream ss;
			ss << filename << "_" << counter;
			newFilename = ss.str();
			counter++;
		} while (fileExists(newFilename));

		filename = newFilename;
	}
	return filename;
}

void uploadLogs(const std::string& content) {
	std::ofstream	logFd("logs/uploads.log", std::ios_base::app);
	logFd << content << std::endl;
}
