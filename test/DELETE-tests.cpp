
#include "tests.hpp"
#include "Config.hpp"
#include "Response.hpp"

bool fileExists(const std::string& filename) {
	struct stat buffer;
	return (stat(filename.c_str(), &buffer) == 0);
}

bool directoryExists(const std::string& path) {
	struct stat buffer;
	return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
}


TEST(DELETEstatusCodes, DeleteFile) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ARRANGE: Criar um arquivo no diretório padrão
	std::string filename = std::string(DEFAULT_ROOT_DIRECTORY) + "/delete_test_file.txt";
	std::ofstream file(filename);
	file << "Conteúdo do arquivo para teste de DELETE.";
	file.close();
	ASSERT_TRUE(fileExists(filename)) << "Arquivo não foi criado corretamente.";

	// ACT: Enviar uma requisição DELETE para o arquivo criado
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/delete_test_file.txt");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar se a requisição DELETE foi bem-sucedida e se o arquivo foi deletado
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 204);  // Supondo que o servidor retorna 200 OK ao deletar um arquivo com sucesso
	EXPECT_FALSE(fileExists(filename)) << "Arquivo não foi deletado.";

	// Limpeza: Apagar o arquivo manualmente se ainda existir
	if (fileExists(filename)) {
		unlink(filename.c_str());  // remove o arquivo
	}

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(DELETEstatusCodes, DeleteNonExistentFile) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT: Tentar deletar um arquivo que não existe
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/nonexistent_file.txt");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(response.body.find(NOT_FOUND_ERROR) != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(DELETEstatusCodes, DeleteWithoutPermission) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ARRANGE: Criar um arquivo e definir permissões do diretório pai para que o arquivo não possa ser deletado
	std::string directory = std::string(DEFAULT_ROOT_DIRECTORY) + "/files/";
	std::string filename = directory + "no_permission_file.txt";

	// Cria o diretório se não existir
	if (!directoryExists(directory)) {
		mkdir(directory.c_str(), 0755);
	}

	// Cria o arquivo
	std::ofstream file(filename);
	file << "Conteúdo para teste de permissão.";
	file.close();

	// Definir permissões de escrita apenas para o diretório pai
	chmod(directory.c_str(), 0555); // Remove permissões de escrita do diretório pai

	ASSERT_TRUE(fileExists(filename)) << "Arquivo não foi criado corretamente.";

	// ACT: Tentar deletar o arquivo sem permissão
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/files/no_permission_file.txt");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar se o servidor retorna o código de erro apropriado (ex: 403 Forbidden)
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 403);
	EXPECT_TRUE(response.body.find("Forbidden") != std::string::npos);

	// Limpeza: Reverter permissões do diretório e apagar o arquivo manualmente
	chmod(directory.c_str(), 0755);

	if (fileExists(filename)) {
		unlink(filename.c_str());
	}

	// Verifica se o diretório ainda está vazio antes de tentar removê-lo
	if (directoryExists(directory) && !fileExists(filename)) {
		rmdir(directory.c_str());
	}

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(DELETEstatusCodes, DeleteDirectory) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ARRANGE: Criar um diretório e um arquivo dentro dele
	std::string directory = std::string(DEFAULT_ROOT_DIRECTORY) + "/delete_test_directory/";
	std::string filename = directory + "test_file.txt";

	// Cria o diretório e o arquivo
	mkdir(directory.c_str(), 0755);
	std::ofstream file(filename);
	file << "Conteúdo do arquivo para teste de DELETE no diretório.";
	file.close();

	ASSERT_TRUE(directoryExists(directory)) << "Diretório não foi criado corretamente.";
	ASSERT_TRUE(fileExists(filename)) << "Arquivo dentro do diretório não foi criado corretamente.";

	// ACT: Enviar uma requisição DELETE para o diretório
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/delete_test_directory/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar se a requisição DELETE foi bem-sucedida e se o diretório e o arquivo foram deletados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 204);  // Supondo que o servidor retorna 204 No Content ao deletar um diretório com sucesso
	EXPECT_FALSE(directoryExists(directory)) << "Diretório não foi deletado.";
	EXPECT_FALSE(fileExists(filename)) << "Arquivo dentro do diretório não foi deletado.";

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(DELETEstatusCodes, DeleteNonExistentDirectory) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT: Tentar deletar um diretório que não existe
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/nonexistent_directory/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(response.body.find(NOT_FOUND_ERROR) != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(DELETEstatusCodes, DeleteForbiddenDirectory) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ARRANGE: Criar um diretório e um arquivo dentro dele
	std::string directory = std::string(DEFAULT_ROOT_DIRECTORY) + "/forbidden_directory/";
	std::string filename = directory + "test_file.txt";

	// Cria o diretório e o arquivo
	mkdir(directory.c_str(), 0755);
	std::ofstream file(filename);
	file << "Conteúdo para teste de permissão em diretório.";
	file.close();

	// Definir permissões de leitura apenas para o diretório pai
	chmod(directory.c_str(), 0555); // Remove permissões de escrita do diretório

	ASSERT_TRUE(directoryExists(directory)) << "Diretório não foi criado corretamente.";
	ASSERT_TRUE(fileExists(filename)) << "Arquivo dentro do diretório não foi criado corretamente.";

	// ACT: Tentar deletar o diretório sem permissão
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/forbidden_directory/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar se o servidor retorna o código de erro apropriado (ex: 403 Forbidden)
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 403);
	EXPECT_TRUE(response.body.find("Forbidden") != std::string::npos);

	// Limpeza: Reverter permissões do diretório e excluir o arquivo
	chmod(directory.c_str(), 0755); // Reverte as permissões

	if (fileExists(filename)) {
		unlink(filename.c_str());
	}

	if (directoryExists(directory) && !fileExists(filename)) {
		rmdir(directory.c_str());
	}

	curl_easy_cleanup(curl);
	stop_server();
}
