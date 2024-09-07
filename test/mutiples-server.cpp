
#include "tests.hpp"

// curl request to domains
TEST(mutiplesServers, serverName) {
	// ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/server_names/servers_config.conf");
	curl = curl_easy_init();
	struct curl_slist *host = nullptr;
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://domain2.com:8090/home");
	host = curl_slist_append(host, "domain2.com:8090:127.0.0.1");
	curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
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

TEST(mutiplesServers, serverNameRedirCurl) {
	// ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/server_names/servers_config.conf");
	curl = curl_easy_init();
	struct curl_slist *host = nullptr;
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://domain1.com:8080/redir-to-home");
	host = curl_slist_append(host, "domain1.com:8080:127.0.0.1");
	curl_easy_setopt(curl, CURLOPT_RESOLVE, host);
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
	EXPECT_EQ(response.headers["Location"], "http://domain2.com:8090/home");

	curl_easy_cleanup(curl);
	stop_server();
}
