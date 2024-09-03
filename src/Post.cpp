#include "Post.hpp"
#include "Response.hpp"

Post::Post(Response &objectRef) : response(objectRef) {}

Post::~Post(void) {}

bool writeFile(const std::string& content, const std::string& fileName);

int	Post::handleRequest(void)
{
	// verifica se é cgi
		// handleCGI
	// rota valida
		// upload
	// qualquer outra coisa
		// bad request
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
			if (!writeFile(response.requestInfo.multipartValues[0], filename))
				return (500);
			return (201);
		}
	}
	return (404);
}

bool writeFile(const std::string& content, const std::string& fileName) {
	std::ofstream file(fileName.c_str());
	if (file.is_open()) {
		file << content;
		file.close();
		std::cerr << "[ParseBodyInfo.cpp - writefile] File Writed!" << std::endl;
		return true;
	} else {
		std::cerr << "[ParseBodyInfo.cpp - writefile] Error at open file!" << std::endl;
	}
	return false;
}

void	Post::upload(void)
{
	// Handle file upload logic here
}

void	Post::buildBody(void) {
	std::string body = response.getDefaultPage();

	if (body == NO_DEFAULT_ERROR)
		response.setBody(CREATED_SUCCESSFULLY);
	response.setBody(body);
}
