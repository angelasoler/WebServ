#include <gtest/gtest.h>
#include <curl/curl.h>
#include <thread>
#include <chrono>

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
	((std::string*)userp)->append((char*)contents, size * nmemb);
	return size * nmemb;
}

void start_server() {
	std::system("./webserv &");
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

// TO-DO
// [_]Fazer rodar curl corretamente ou com telnet
// [_]matar processo do webserv antes de terminar
TEST(HttpTest, GetRequest) {
	CURL* curl;
	CURLcode res;
	std::string readBuffer;

	start_server();


	curl = curl_easy_init();
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);

		EXPECT_EQ(res, CURLE_OK);
		EXPECT_TRUE(readBuffer.find("Hello\n") != std::string::npos);
	}
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

