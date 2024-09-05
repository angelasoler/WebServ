#include "tests.hpp"
#include "Config.hpp"
#include "Response.hpp"

bool thisFileExists(const std::string& filename) {
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

bool areFilesIdentical(const std::string& p1, const std::string& p2) {
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

TEST(POSTuploadFile, UploadFile) {
	HttpResponse response;
	CURL* curl;
	start_server("test/upload-tests/upload-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// Configurações para o POST com arquivo
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/uploads");
	curl_mime* mime;
	curl_mimepart* part;
	mime = curl_mime_init(curl);

	// Adiciona o arquivo ao POST
	std::string filename = "FileRandom";
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
	std::string uploadedFile = std::string(DEFAULT_ROOT_DIRECTORY) + "/" + "uploads" + "/" + filename;
	EXPECT_TRUE(thisFileExists(uploadedFile.c_str())); // << "File not created\n" << unlink(filename.c_str());
	EXPECT_TRUE(areFilesIdentical(filename, uploadedFile)); // << "Uploaded File are not the same\n" << unlink(uploadedFile.c_str()) << unlink(filename.c_str());
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 201);
	EXPECT_TRUE(response.body.find(CREATED_SUCCESSFULLY) != std::string::npos);

	// Limpeza
	unlink(filename.c_str());
	unlink(uploadedFile.c_str());

	curl_mime_free(mime);
	curl_easy_cleanup(curl);
	stop_server();
}

TEST(POSTuploadFile, UploadThreeFiles) {
	HttpResponse response;
	CURL* curl;
	start_server("test/upload-tests/upload-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// Configurações para o POST com múltiplos arquivos
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/uploads");
	curl_mime* mime = curl_mime_init(curl);

	// Criação e adição dos arquivos ao POST
	std::vector<std::string> filenames = {"FileRandom1", "FileRandom2", "FileRandom3"};
	std::vector<std::string> fileContents = {
		"conteúdo do arquivo 1",
		"conteúdo do arquivo 2",
		"conteúdo do arquivo 3"
	};

	for (size_t i = 0; i < filenames.size(); ++i) {
		// Cria e escreve conteúdo nos arquivos
		if (!createAndWriteFile(filenames[i], fileContents[i])) {
			std::cerr << "Error creating file: " << filenames[i] << "\n";
		}

		// Adiciona o arquivo ao MIME part
		curl_mimepart* part = curl_mime_addpart(mime);
		curl_mime_name(part, ("file" + std::to_string(i + 1)).c_str());  // Define um nome único para cada arquivo
		curl_mime_filedata(part, filenames[i].c_str());
	}

	// Configura o MIME no curl
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

	// Executa o POST
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// Verifica o resultado para cada arquivo
	for (size_t i = 0; i < filenames.size(); ++i) {
		std::string uploadedFile = std::string(DEFAULT_ROOT_DIRECTORY) + "/" + "uploads" + "/" + filenames[i];
		EXPECT_TRUE(thisFileExists(uploadedFile.c_str())) << "File not created: " << filenames[i];
		EXPECT_TRUE(areFilesIdentical(filenames[i], uploadedFile)) << "Uploaded file is not the same as original: " << filenames[i];
	}

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 201);
	EXPECT_TRUE(response.body.find(CREATED_SUCCESSFULLY) != std::string::npos);

	// Limpeza
	for (size_t i = 0; i < filenames.size(); ++i) {
		unlink(filenames[i].c_str());
		std::string uploadedFile = std::string(DEFAULT_ROOT_DIRECTORY) + "/" + "uploads" + "/" + filenames[i];
		unlink(uploadedFile.c_str());
	}

	curl_mime_free(mime);
	curl_easy_cleanup(curl);
	stop_server();
}
