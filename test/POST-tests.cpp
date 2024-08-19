
// #include "tests.hpp"

// TEST(CurlHttpTest, PostRequest200) {
// 	// ARRANGE: Configuração do teste e inicializaçãos
// 	HttpResponse response;
// 	CURL* curl;
// 	start_server("");
// 	curl = curl_easy_init();
// 	ASSERT_NE(curl, nullptr);

// 	 // ACT: Executar a ação sob teste
// 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
// 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
// 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
// 	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
// 	CURLcode res = curl_easy_perform(curl);
// 	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


// 	 // ASSERT: Verificar os resultados
// 	EXPECT_EQ(res, CURLE_OK);
// 	EXPECT_EQ(response.status_code, 200);
// 	EXPECT_TRUE(response.body.find("Hello, World!") != std::string::npos);

// 	curl_easy_cleanup(curl);
// 	stop_server();
// }

// TEST(CurlHttpTest, PostRequest404) {
// 	HttpResponse response;
// 	CURL* curl;
// 	start_server("");
// 	curl = curl_easy_init();
// 	ASSERT_NE(curl, nullptr);

// 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/notfound.html");
// 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
// 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
// 	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
// 	CURLcode res = curl_easy_perform(curl);
// 	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

// 	EXPECT_EQ(res, CURLE_OK);
// 	EXPECT_EQ(response.status_code, 404);
// 	EXPECT_TRUE(response.body.find("Not Found") != std::string::npos);

// 	curl_easy_cleanup(curl);
// 	stop_server();
// }

// TEST(CurlHttpTest, PostRequest301) {
// 	HttpResponse response;
// 	CURL* curl;
// 	start_server("");
// 	curl = curl_easy_init();
// 	ASSERT_NE(curl, nullptr);
// 	std::string url = std::string("http://localhost:8080/") + DEFAULT_REDIRECTION;
	
// 	curl_easy_setopt(curl, CURLOPT_URL, url); // É necessario inserir o redirect configurado
// 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
// 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
// 	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
// 	CURLcode res = curl_easy_perform(curl);
// 	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

// 	EXPECT_EQ(res, CURLE_OK);
// 	EXPECT_EQ(response.status_code, 301);
// 	EXPECT_TRUE(response.headers.count("Location") > 0);

// 	curl_easy_cleanup(curl);
// 	stop_server();
// }

// TEST(CurlHttpTest, PostRequest403) {
// 	HttpResponse response;
// 	CURL* curl;
// 	start_server("");
// 	curl = curl_easy_init();
// 	ASSERT_NE(curl, nullptr);
// 	std::string forbiddenFile = DEFAULT_ROOT_DIRECTORY + std::string("/forbiddenFile"); // define um arquivo a partir do diretorio root padrao
// 	std::ofstream file(forbiddenFile.c_str()); // cria o arquivo
// 	chmod(forbiddenFile.c_str(), 0000); // define as permissoes para 0000 

// 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/forbiddenFile"); // tenta acessar o arquivo
// 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
// 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
// 	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
// 	CURLcode res = curl_easy_perform(curl);
// 	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

// 	EXPECT_EQ(res, CURLE_OK);
// 	EXPECT_EQ(response.status_code, 403);
// 	EXPECT_TRUE(response.body.find("Forbidden") != std::string::npos);

// 	remove(forbiddenFile.c_str()); // exclui o arquivo proibido criado
// 	curl_easy_cleanup(curl);
// 	stop_server();
// }

// TEST(CurlHttpTest, PostRequest400) {
// 	HttpResponse response;
// 	CURL* curl;
// 	start_server("");
// 	curl = curl_easy_init();
// 	ASSERT_NE(curl, nullptr);

// 	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/badrequest");
// 	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
// 	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
// 	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
// 	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
// 	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
// 	CURLcode res = curl_easy_perform(curl);
// 	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

// 	EXPECT_EQ(res, CURLE_OK);
// 	EXPECT_EQ(response.status_code, 400);
// 	EXPECT_TRUE(response.body.find("Bad Request") != std::string::npos);

// 	curl_easy_cleanup(curl);
// 	stop_server();
// }
