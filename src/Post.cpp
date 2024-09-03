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
	if (response.requestInfo.pathType == URL)
		return upload();
	return (405);
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

std::string	Post::getFileName(void)
{
	RequestInfo	&info = response.requestInfo;
	std::string filename;
	std::string header = info.multipartHeaders[0];
	size_t pos = header.find("filename=");

	if (pos != std::string::npos) {
		size_t start = header.find("\"", pos + 5) + 1;
		size_t end = header.find("\"", start);
		filename = info.configRef.root_directory + "/" + info.configRef.upload_directory + "/" + header.substr(start, end - start);
		return filename;
	}
    // Se o filename foi encontrado
    if (!filename.empty()) {
        filename = info.configRef.root_directory + "/" + info.configRef.upload_directory + "/" + filename;
    } else {
        filename = info.configRef.root_directory + "/" + info.configRef.upload_directory + "/new_file";
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

int	Post::upload(void)
{
	RequestInfo	&info = response.requestInfo;

	if (info.contentType.find("multipart/form-data") != std::string::npos)
	{
		if (info.multipartHeaders.size() < 1 || info.multipartValues.size() < 1)
			return 202;

		std::string filename = getFilename();
		if (!writeFile(info.multipartValues[0], filename))
			return (500);
		return (201);
	}
}

void	Post::buildBody(void) {
	std::string body = response.getDefaultPage();

	if (body == NO_DEFAULT_ERROR)
		response.setBody(CREATED_SUCCESSFULLY);
	response.setBody(body);
}

static bool fileExists(const std::string& filename)
{
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}
