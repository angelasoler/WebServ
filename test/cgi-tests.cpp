#include "tests.hpp"

//não mandou o dado do file, requisição com metadados vazia
TEST(CGITests, PostUploadPyFail) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/upload");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 500);
	EXPECT_TRUE(response.body.find("ERROR") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

//requisição completa
TEST(CGITests, PostUploadPy) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	curl_mime *form = curl_mime_init(curl);
	curl_mimepart *field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, "test/cgi-tests/upload-thisfile.txt");
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/upload/upload.py");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 201);
	EXPECT_TRUE(response.body.find("Upload bem-sucedido") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

// requisição bem sucedida
TEST(CGITests, GETCalculatorPy) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/calculator/calculator.py?num1=10&num2=5&operation=add");
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
	EXPECT_TRUE(response.body.find("15.0") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

// bad query string
TEST(CGITests, GETCalculatorPyFail) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/calculator/calculator.py?num1=adsad&num2=asdasd&operation=add");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 500);
	EXPECT_TRUE(response.body.find("ERROR") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CGITests, GETCalculatorPyBadQueryString) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/calculator/calculator.py?dasfafafasdf");
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
	EXPECT_TRUE(response.body.find("0.0") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CGITests, GETNotAllowed) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/no_permission/cantexec.py?dasfafafasdf");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 405);
	EXPECT_TRUE(response.body.find("Method Not Allowed") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

TEST(CGITests, GETInfinitLoop) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/cgi-tests/cgi-config.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/no_permission/infinit_loop.py?dasfafafasdf");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 500);
	EXPECT_TRUE(response.body.find("Error") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

// notfound
// http://localhost:8080/cgi-bin/infinit_loop.py
