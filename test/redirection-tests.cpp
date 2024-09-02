#include "tests.hpp"

TEST(RedirectionTests, invalidRedir) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/redir-tests/redir-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/invalid-redir");
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
	EXPECT_EQ(response.headers["Location"], "some-crap");

	curl_easy_cleanup(curl);
	stop_server();
}

//TO-DO testes com varios servidores e servernames, com redirections entre eles
// melhro com fire-fos/selenium pois com curl não veremos o redirection como tal
// ou seja o proximos get resultante do redirection