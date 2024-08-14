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

TEST(CurlHttpTest, GetRequest200) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);


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

TEST(CurlHttpTest, GetRequest404) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/notfound");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(response.body.find("Not Found") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest500) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/error");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 500);
	EXPECT_TRUE(response.body.find("Internal Server Error") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest301) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/oldpage");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 301);
	EXPECT_TRUE(response.headers.count("Location") > 0);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequestFollowRedirect) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/redirect");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L); // Segue redirecionamentos
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(response.body.find("Final destination") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest403) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/forbidden");
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

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest400) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/badrequest");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 400);
	EXPECT_TRUE(response.body.find("Bad Request") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest401) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/unauthorized");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 401);
	EXPECT_TRUE(response.body.find("Unauthorized") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest502) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/badgateway");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 502);
	EXPECT_TRUE(response.body.find("Bad Gateway") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CurlHttpTest, GetRequest503) {
	HttpResponse response;
	CURL* curl;
	start_server();
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/unavailable");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 503);
	EXPECT_TRUE(response.body.find("Service Unavailable") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}
