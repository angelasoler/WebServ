
#include "tests.hpp"
#include "ParsePathInfo.hpp"

RequestInfo parseHttpRequest(const std::string& httpRequest)
{
	int 			sockfd[2];
	Request			request;
	Config  		*config = Config::getInstance();

	config->loadDefaultConfig();
	ServerConfig server = config->servers[0];

	// Cria um par de sockets conectados
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd) == -1) {
		perror("[request-info-tests.cpp - parseHttpRequest] socketpair");
	}

	// Envia a string de teste para o socket
	if (send(sockfd[0], httpRequest.c_str(), strlen(httpRequest.c_str()), 0) == -1) {
		perror("[request-info-tests.cpp - parseHttpRequest] send");
	}

	close(sockfd[0]);
	request.readRequest(sockfd[1]);
	close(sockfd[1]);

	request.parseRequest(server);
	return request.info;
}


TEST(RequestInfoTest, HandlesGetRequest) {
	std::string getRequest = 
		"GET /index.html HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Connection: close\r\n\r\n";

	RequestInfo requestInfo = parseHttpRequest(getRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/index.html");
	EXPECT_EQ(requestInfo.action, RESPONSE);
	EXPECT_EQ(requestInfo.pathType, File);
	EXPECT_EQ(requestInfo.permissions.read, true);
	ASSERT_TRUE(requestInfo.body.empty());
	ASSERT_TRUE(requestInfo.urlencodedValues.empty());
}

TEST(RequestInfoTest, HandlesDeleteRequest) {
	std::string deleteRequest = 
		"DELETE " + std::string(DEFAULT_FILE) + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Connection: close\r\n\r\n";

	RequestInfo requestInfo = parseHttpRequest(deleteRequest);

	// Verificando o preenchimento da struct
	EXPECT_EQ(requestInfo.requestedRoute, DEFAULT_FILE);
	EXPECT_EQ(requestInfo.action, DELETE);
	EXPECT_EQ(requestInfo.pathType, File);
	EXPECT_EQ(requestInfo.permissions.read, true);
	EXPECT_EQ(requestInfo.permissions.write, true);
	ASSERT_TRUE(requestInfo.body.empty());
	ASSERT_TRUE(requestInfo.urlencodedValues.empty());
}

#include "PrintRequestInfo.hpp"
RequestInfo parsePOSTHttpRequest(const std::string& httpRequest)
{
	int 			sockfd[2];
	Request			request;
	Config  		*config = Config::getInstance();

	config->loadConfig("test/upload-tests/upload-config.conf");
	printConfig();
	ServerConfig server = config->servers[0];

	// Cria um par de sockets conectados
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd) == -1) {
		perror("[request-info-tests.cpp - parsePOSTHttpRequest] socketpair");
	}
	
	// Envia a string de teste para o socket
	if (send(sockfd[0], httpRequest.c_str(), strlen(httpRequest.c_str()), 0) == -1) {
		perror("[request-info-tests.cpp - parsePOSTHttpRequest] send");
	}
	close(sockfd[0]);
	request.readRequest(sockfd[1]);
	close(sockfd[1]);

	request.parseRequest(server);
	return request.info;
}

TEST(RequestInfoTest, HandlesPostRequest) {
	std::string postRequest = 
		"POST " + std::string("/uploads") + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 27\r\n\r\n"
		"name=John&age=30&city=NYC";

	RequestInfo requestInfo = parsePOSTHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.body, "name=John&age=30&city=NYC");
	EXPECT_EQ(requestInfo.urlencodedValues["name"], "John");
	EXPECT_EQ(requestInfo.urlencodedValues["age"], "30");
	EXPECT_EQ(requestInfo.urlencodedValues["city"], "NYC");
}

TEST(RequestInfoTest, HandlesPostRequestWithFormUrlEncoded) {
	std::string postRequest = 
		"POST " + std::string("/uploads") + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 29\r\n\r\n"
		"username=testuser&age=34";

	
	RequestInfo requestInfo = parsePOSTHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.body, "username=testuser&age=34");
	EXPECT_EQ(requestInfo.urlencodedValues["username"], "testuser");
	EXPECT_EQ(requestInfo.urlencodedValues["age"], "34");
}

TEST(RequestInfoTest, HandlesPostRequestWithHtmlBody) {
	std::string postRequest = 
		"POST /uploads HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 51\r\n\r\n"
		"<html><body><p>This is a test</p></body></html>";

	RequestInfo requestInfo = parsePOSTHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.body, "<html><body><p>This is a test</p></body></html>");
}

TEST(RequestInfoTest, HandlesPostRequestWithMultipartFormData) {
	std::string boundary = "--boundary";
	std::string postRequest = 
		"POST /uploads HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
		"Content-Length: 190\r\n\r\n" +
		boundary + "\r\n" +
		"Content-Disposition: form-data; name=\"field1\"\r\n\r\n" +
		"VALUE 1\r\n" + 
		boundary + "\r\n" +
		"Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n" +
		"Content-Type: text/plain\r\n\r\n" +
		"VALUE 2\r\n" + 
		boundary + "--";

	RequestInfo requestInfo = parsePOSTHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.permissions.read, true);
	EXPECT_EQ(requestInfo.permissions.write, true);
	EXPECT_EQ(requestInfo.permissions.execute, false);
	ASSERT_EQ(requestInfo.multipartHeaders.size(), 2);
	ASSERT_EQ(requestInfo.multipartValues.size(), 2);
	EXPECT_TRUE(requestInfo.multipartValues[0].find("VALUE 1") != std::string::npos);
	EXPECT_TRUE(requestInfo.multipartValues[1].find("VALUE 2") != std::string::npos);
	EXPECT_TRUE(requestInfo.multipartHeaders[0].find("name=\"field1\"") != std::string::npos);
	EXPECT_TRUE(requestInfo.multipartHeaders[1].find("filename=\"test.txt\"") != std::string::npos);
}

TEST(RequestInfoTest, HandlesPostRequestWithChunkedHtmlBody) {
	std::string chunkedPostRequest = 
		"POST /uploads HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Content-Type: text/html\r\n\r\n"
		"37\r\n"
		"<html><body><p>This is a chunked test</p></body></html>\r\n"
		"0\r\n\r\n";

	RequestInfo requestInfo = parsePOSTHttpRequest(chunkedPostRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.body, "<html><body><p>This is a chunked test</p></body></html>");
}

TEST(RequestInfoTest, HandlesPostRequestWithChunkedFormUrlEncoded) {
	std::string chunkedPostRequest = 
		"POST /uploads HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Transfer-Encoding: chunked\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n\r\n"
		"18\r\n"
		"username=testuser&age=34\r\n"
		"0\r\n\r\n";

	RequestInfo requestInfo = parsePOSTHttpRequest(chunkedPostRequest);

	EXPECT_EQ(requestInfo.requestedRoute, "/uploads");
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.body, "username=testuser&age=34");
	EXPECT_EQ(requestInfo.urlencodedValues["username"], "testuser");
	EXPECT_EQ(requestInfo.urlencodedValues["age"], "34");
}
