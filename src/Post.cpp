#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

void writeFile(const std::string& content, const std::string& fileName);

int	Post::handleRequest(void)
{
	if (response.requestInfo.contentType.find("multipart/form-data") != std::string::npos)
	{
		if (response.requestInfo.multipartHeaders.size() < 1 || response.requestInfo.multipartValues.size() < 1)
			return 202;
		std::string header = response.requestInfo.multipartHeaders[0];
		size_t pos = header.find("filename=");
		if (pos != std::string::npos) {
			size_t start = header.find("\"", pos + 5) + 1;
			size_t end = header.find("\"", start);
			std::string filename = response.requestInfo.configRef.root_directory + "/" + response.requestInfo.configRef.upload_directory + "/" + header.substr(start, end - start);
			writeFile(response.requestInfo.multipartValues[0], filename);
		}
	}
	return (404);
}

void writeFile(const std::string& content, const std::string& fileName) {
	std::ofstream file(fileName.c_str());
	if (file.is_open()) {
		file << content;
		file.close();
		std::cerr << "[ParseBodyInfo.cpp - writefile] File Writed!" << std::endl;
	} else {
		std::cerr << "[ParseBodyInfo.cpp - writefile] Error at open file!" << std::endl;
	}
}

void	Post::upload(void)
{
	// Handle file upload logic here
}

void	Post::buildBody(void) {}
