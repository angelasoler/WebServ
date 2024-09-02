
#include "tests.hpp"
#include "Config.hpp"

TEST(ChunkedRequests, POSTChunkedRequest) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n"); // Simula um request com chunks
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen("4\r\nWiki\r\n5\r\npedia\r\n0\r\n\r\n"));
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Transfer-Encoding: chunked"));

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(response.body.find("Wiki") != std::string::npos);
	EXPECT_TRUE(response.body.find("pedia") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(ChunkedRequests, POSTChunkedRequestCustom) {
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");

	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Transfer-Encoding: chunked");
	headers = curl_slist_append(headers, "Content-Type: text/html");
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

	std::string chunked_request = 
		"33\r\n"
		"<html><body><p>This is a chunked test</p></body></html>\r\n"
		"0\r\n\r\n";

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, chunked_request.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, chunked_request.size());

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(response.body.find("This is a chunked test") != std::string::npos);

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	stop_server();
}
