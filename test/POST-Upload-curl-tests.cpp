#include "tests.hpp"
#include "Config.hpp"

bool thisFileExists(const std::string& filename) {
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

bool areFilesIdentical(const std::string& file1, const std::string& file2) {
    std::ifstream ifs1(file1.c_str(), std::ifstream::binary);
    std::ifstream ifs2(file2.c_str(), std::ifstream::binary);

    if (!ifs1.is_open() || !ifs2.is_open()) {
        return false;
    }

    char ch1, ch2;
    while (ifs1.get(ch1) && ifs2.get(ch2)) {
        if (ch1 != ch2) {
            return false;
        }
    }

    return ifs1.eof() && ifs2.eof();
}

bool createAndWriteFile(const std::string& filename, const std::string& text) {


	std::string filepath = filename;
	std::ofstream file(filepath.c_str());
	if (!file.is_open()) {
		return false;
	}
	file << text;
	file.close();

	return true;
}

TEST(POSTuploadFile, UploadMakefile) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// Configurações para o POST com arquivo
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/");
	curl_mime* mime;
	curl_mimepart* part;
	mime = curl_mime_init(curl);

	// Adiciona o arquivo ao POST
	std::string filename = "./FileRandom";
	std::string	fileContent = "texto\naleatorio\naqui\ndentro1111111111111";
	if (!createAndWriteFile(filename, fileContent))
		std::cerr << "Error\n";
	part = curl_mime_addpart(mime);
	curl_mime_name(part, "file");
	curl_mime_filedata(part, (filename).c_str());

	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

	// Executa o POST
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// Verifica o resultado
	std::string uploadedFile = std::string(DEFAULT_ROUTE_PATH) + "/" + DEFAULT_UPLOAD_DIRECTORY + "/" + filename;
	ASSERT_TRUE(thisFileExists(uploadedFile.c_str())) << "File not created\n" << unlink(filename.c_str());
	ASSERT_TRUE(areFilesIdentical(filename, uploadedFile)) << "Uploaded File are not the same\n" << unlink(filename.c_str());
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 202);
	EXPECT_TRUE(response.body.find("Upload successful") != std::string::npos);

	// Limpeza
	unlink(filename.c_str());
	unlink(uploadedFile.c_str());

	curl_mime_free(mime);
	curl_easy_cleanup(curl);
	stop_server();
}
