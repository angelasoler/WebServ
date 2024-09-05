#include "tests.hpp"

TEST(RedirectionTests, invalidRedir) {
	// ARRANGE
	start_server("test/redir-tests/redir-config.conf");
	// ACT
	int result = std::system("test/scripts/test-invalid-redirection.py");

	// ASSERT
	EXPECT_EQ(result, 0);
	stop_server();
}

// get to http://localhost:8080/redir-to-home
// redirect to http://localhost:8090/home
TEST(RedirectionTests, validRedir) {
	// ARRANGE
	start_server("test/redir-tests/redir-config.conf");
	// ACT
	int result = std::system("test/scripts/test-redirection.py");

	// ASSERT
	EXPECT_EQ(result, 0);
	stop_server();
}

TEST(RedirectionTests, directRequest) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/redir-tests/redir-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8090/home");
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
	EXPECT_TRUE(response.body.find("First server configurated") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}
