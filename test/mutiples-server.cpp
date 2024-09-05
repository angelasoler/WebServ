
#include "tests.hpp"

// curl request to domains
TEST(mutiplesServers, serverName) {
	// ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/server_names/servers_config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://domain2.com/");
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
	EXPECT_TRUE(response.body.find("") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(mutiplesServers, serverNameRedirCurl) {
	// ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/server_names/servers_config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://domain1.com/redir-to-home");
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
	EXPECT_EQ(response.headers["Location"], "http://domain2.com/home");

	curl_easy_cleanup(curl);
	stop_server();
}

// browser request to domains
TEST(mutiplesServers, serverNameRedir) {
	// ARRANGE
	start_server("test/server_names/servers_config.conf");
	// ACT
	int result = std::system("test/scripts/test-server-name.py");

	// ASSERT
	EXPECT_EQ(result, 0);
	stop_server();
}
