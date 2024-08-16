#include <gtest/gtest.h>
#include <curl/curl.h>

void start_server();
void stop_server();

struct HttpResponse {
	long status_code;
	std::map<std::string, std::string> headers;
	std::string body;
};

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

TEST(HttpTest, GetRequest) {
	// ARRANGE: Configuração do teste e inicialização
	HttpResponse response;
	CURL* curl;
	start_server();
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
