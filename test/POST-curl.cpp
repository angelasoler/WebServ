#include "tests.hpp"

TEST(POST_requests, Request201Form) {
	// ARRANGE
	HttpResponse response;
	CURL* curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// Dados a serem enviados no corpo da requisição POST
	std::string postData = "field1=value1&field2=value2";

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_POST, 1L); // Define o método como POST
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str()); // Dados do POST
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 201); // Verifica o status HTTP
	EXPECT_TRUE(response.body.find("Resource created successfully") != std::string::npos);

	curl_easy_cleanup(curl);
}

TEST(POST_requests, Request201Upload) {
	// ARRANGE
	HttpResponse response;
	CURL* curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);


	std::string boundary = "----WebKitFormBoundary7MA4YWxkTrZu0gW";
	std::string postData = "--" + boundary + "\r\n"
						"Content-Disposition: form-data; name=\"field1\"\r\n\r\nvalue1\r\n"
						"--" + boundary + "\r\n"
						"Content-Disposition: form-data; name=\"field2\"\r\n\r\nvalue2\r\n"
						"--" + boundary + "--\r\n";

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, {"Content-Type: multipart/form-data; boundary=" + boundary});

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_POST, 1L); // Define o método como POST
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.c_str()); // Dados do POST
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 201); // Verifica o status HTTP
	EXPECT_TRUE(response.body.find("Resource created successfully") != std::string::npos);

	curl_easy_cleanup(curl);
}