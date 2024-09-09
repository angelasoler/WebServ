
#include "tests.hpp"
#include "Config.hpp"
#include "Response.hpp"

TEST(ChunkedRequests, POSTChunkedRequest) {
	HttpResponse response;
	CURL* curl;
	start_server("test/upload-tests/upload-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n");
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen("4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n"));
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Transfer-Encoding: chunked"));

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 400);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(ChunkedRequests, POSTChunkedRequestCustom) {
	HttpResponse response;
	CURL* curl;
	start_server("test/upload-tests/upload-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Transfer-Encoding: chunked");
	headers = curl_slist_append(headers, "Content-Type: text/html");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string chunked_request = 
		"33\r\n"
		"<html><body><p>This is a chunked test</p></body></html>\r\n"
		"0\r\n\r\n";

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, chunked_request.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, chunked_request.size());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 400);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	stop_server();
}

static bool createAndWriteFile(const std::string& filename, const std::string& text) {


	std::string filepath = filename;
	std::ofstream file(filepath.c_str());
	if (!file.is_open()) {
		return false;
	}
	file << text;
	file.close();

	return true;
}

static bool thisFileExists(const std::string& filename) {
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

static bool areFilesIdentical(const std::string& p1, const std::string& p2) {
	std::ifstream f1(p1, std::ifstream::binary|std::ifstream::ate);
	std::ifstream f2(p2, std::ifstream::binary|std::ifstream::ate);

	if (f1.fail() || f2.fail()) {
		return false; //file problem
	}

	if (f1.tellg() != f2.tellg()) {
		return false; //size mismatch
	}

	//seek back to beginning and use std::equal to compare contents
	f1.seekg(0, std::ifstream::beg);
	f2.seekg(0, std::ifstream::beg);
	return std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
						std::istreambuf_iterator<char>(),
						std::istreambuf_iterator<char>(f2.rdbuf()));
}

#include <dirent.h>
static int directorySize(const std::string& path) {
    DIR* dir = opendir(path.c_str());
    if (dir == NULL) {
        std::cerr << "Error at open directory: " << path << std::endl;
        return -1;
    }

    struct dirent* entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (std::string(entry->d_name) != "." && std::string(entry->d_name) != "..") {
            ++count;
        }
    }

    closedir(dir);
    return count;
}

TEST(ChunkedRequests, POSTChunkedMultipartFormData) {
	HttpResponse response;
	CURL* curl;
	int initDirSize;
	
	start_server("test/upload-tests/upload-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/uploads");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Transfer-Encoding: chunked");
	headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);


	// Adiciona o arquivo ao POST
	std::string filename = "FileRandom";
	std::string	fileContent = "texto\naleatorio\naqui\ndentro1111111111111";
	std::string uploadDir = std::string(DEFAULT_ROOT_DIRECTORY) + "/" + "uploads" + "/";
	std::string uploadedFile = uploadDir + filename;
	initDirSize = directorySize(uploadDir);
	if (!createAndWriteFile(filename, fileContent))
		std::cerr << "Error\n";

	// Configurando o formulário de dados para enviar o arquivo
	curl_mime* form = curl_mime_init(curl);
	curl_mimepart* field = curl_mime_addpart(form);
	
	// Nome do campo no formulário e o arquivo a ser enviado
	curl_mime_name(field, "file1");
	curl_mime_filedata(field, filename.c_str());

	// Definindo o mime/form como dados de envio
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);

	// Configurações para capturar a resposta
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// std::cerr <
	EXPECT_EQ(res, CURLE_OK);
	(void)initDirSize;
	EXPECT_EQ(initDirSize + 1, directorySize(uploadDir));
	EXPECT_TRUE(thisFileExists(uploadedFile.c_str())) << "File not created\n";
	EXPECT_FALSE(thisFileExists((uploadDir + "new_file").c_str())) << "ERROR Extra File created\n";
	EXPECT_TRUE(areFilesIdentical(filename, uploadedFile)) << "Uploaded File are not the same\n";
	EXPECT_EQ(response.status_code, 201);
	EXPECT_TRUE(response.body.find(CREATED_SUCCESSFULLY) != std::string::npos);

	// Limpeza
	unlink(filename.c_str());
	EXPECT_TRUE(unlink(uploadedFile.c_str())) << "Uploaded File Fd not Closed by server";
	unlink((uploadDir + "new_file").c_str());
	
	curl_slist_free_all(headers);
	curl_mime_free(form);
	curl_easy_cleanup(curl);
	stop_server();
	unlink(filename.c_str());
	unlink(uploadedFile.c_str());
	unlink((uploadDir + "new_file").c_str());
}
