
#include "tests.hpp"
#include "Config.hpp"

size_t HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata) {
	size_t total_size = size * nitems;
	HttpResponse* response = static_cast<HttpResponse*>(userdata);

	std::string header(buffer, total_size);
	size_t separator = header.find(":");
	if (separator != std::string::npos) {
		std::string key = header.substr(0, separator);
		std::string value = header.substr(separator + 2, header.size() - separator - 4); // Remove '\r\n'
		response->headers[key] = value;
	}
	return total_size;
}

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	size_t total_size = size * nmemb;
	HttpResponse* response = static_cast<HttpResponse*>(userp);
	response->body.append((char*)contents, total_size);
	return total_size;
}

TEST(GETstatusCodes, Request200) {
	// ARRANGE: Configuração do teste e inicializaçãos
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	 // ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


	 // ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(response.body.find("Hello, World!") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(GETstatusCodes, Request404) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/notfound.html");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(response.body.find("not found!") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(GETstatusCodes, Request307) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/redir-tests/redir-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/redir-to-home");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 307);
	EXPECT_EQ(response.headers["Location"], "http://localhost:8090/home");

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(GETstatusCodes, Request403) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);
	std::string forbiddenFile = DEFAULT_ROOT_DIRECTORY + std::string("/forbiddenFile"); // define um arquivo a partir do diretorio root padrao
	std::ofstream file(forbiddenFile.c_str()); // cria o arquivo
	chmod(forbiddenFile.c_str(), 0000); // define as permissoes para 0000 

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/forbiddenFile"); // tenta acessar o arquivo
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 403);
	EXPECT_TRUE(response.body.find("Forbidden") != std::string::npos);

	remove(forbiddenFile.c_str()); // exclui o arquivo proibido criado
	curl_easy_cleanup(curl);
	stop_server();
}

///////////////
// https://developer.mozilla.org/en-US/docs/Web/HTTP/Status/400
// bad request is typically due to malformed request syntax, 
// invalid request message framing, or deceptive request routing.
///////////////
TEST(GETstatusCodes, Request400) {
	// ARRANGE
	start_server("");

	// ACT
	std::string response = exec("test/scripts/test-telnet-400.sh");
	stop_server();

	//ASSERT
	EXPECT_NE(response.find("400"), std::string::npos);
	EXPECT_NE(response.find("ERROR"), std::string::npos);
}
