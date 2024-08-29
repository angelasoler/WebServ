
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
	ASSERT_TRUE(requestInfo.bodyValues.empty());
}

TEST(RequestInfoTest, HandlesPostRequest) {
	std::string postRequest = 
		"POST " + std::string(DEFAULT_ROUTE_PATH) + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 27\r\n\r\n"
		"name=John&age=30&city=NYC";

	RequestInfo requestInfo = parseHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(requestInfo.action, UPLOAD);
	// EXPECT_EQ(requestInfo.pathType, URL);
	EXPECT_EQ(requestInfo.body, "name=John&age=30&city=NYC");
	EXPECT_EQ(requestInfo.bodyValues["name"], "John");
	EXPECT_EQ(requestInfo.bodyValues["age"], "30");
	EXPECT_EQ(requestInfo.bodyValues["city"], "NYC");
}

TEST(RequestInfoTest, HandlesPostRequestWithFormUrlEncoded) {
	std::string postRequest = 
		"POST " + std::string(DEFAULT_ROUTE_PATH) + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 29\r\n\r\n"
		"username=testuser&age=34";

	
	RequestInfo requestInfo = parseHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(requestInfo.action, UPLOAD);
	// EXPECT_EQ(requestInfo.pathType, URL);
	EXPECT_EQ(requestInfo.body, "username=testuser&age=34");
	EXPECT_EQ(requestInfo.bodyValues["username"], "testuser");
	EXPECT_EQ(requestInfo.bodyValues["age"], "34");
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
	ASSERT_TRUE(requestInfo.bodyValues.empty());
}

TEST(RequestInfoTest, HandlesPostRequestWithHtmlBody) {
	std::string postRequest = 
		"POST " + std::string(DEFAULT_ROUTE_PATH) + " HTML HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 51\r\n\r\n"
		"<html><body><p>This is a test</p></body></html>";

	RequestInfo requestInfo = parseHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.permissions.read, true);
	EXPECT_EQ(requestInfo.permissions.write, true);
	EXPECT_EQ(requestInfo.permissions.execute, false);
	EXPECT_EQ(requestInfo.body, "<html><body><p>This is a test</p></body></html>");
}

TEST(RequestInfoTest, HandlesPostRequestWithMultipartFormData) {
	std::string boundary = "--boundary";
	std::string postRequest = 
		"POST " + std::string(DEFAULT_ROUTE_PATH) + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: multipart/form-data; boundary=" + boundary + "\r\n"
		"Content-Length: 144\r\n\r\n" +
		boundary + "\r\n" +
		"Content-Disposition: form-data; name=\"field1\"\r\n\r\n" +
		"value1\r\n" + 
		boundary + "\r\n" +
		"Content-Disposition: form-data; name=\"file\"; filename=\"test.txt\"\r\n" +
		"Content-Type: text/plain\r\n\r\n" +
		"file content here\r\n" + 
		boundary + "--";

	RequestInfo requestInfo = parseHttpRequest(postRequest);

	EXPECT_EQ(requestInfo.requestedRoute, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(requestInfo.action, UPLOAD);
	EXPECT_EQ(requestInfo.permissions.read, true);
	EXPECT_EQ(requestInfo.permissions.write, true);
	EXPECT_EQ(requestInfo.permissions.execute, false);
	EXPECT_TRUE(requestInfo.body.find("value1") != std::string::npos);
	EXPECT_TRUE(requestInfo.body.find("file content here") != std::string::npos);
	EXPECT_EQ(requestInfo.boundary, boundary);
}
