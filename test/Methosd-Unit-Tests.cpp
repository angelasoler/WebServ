# include "tests.hpp"
# include "Request.hpp"
# include "IHttpMethod.hpp"
# include "Get.hpp"
# include "Response.hpp"

TEST(IHttpMethods, PostRequestInfor) {
	// ARRANGE
	std::string postRequest = 
		"POST " + std::string(DEFAULT_ROUTE_PATH) + " HTTP/1.1\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/x-www-form-urlencoded\r\n"
		"Content-Length: 24\r\n\r\n"
		"username=testuser&age=34";

	RequestInfo requestInfo = parseHttpRequest(postRequest);
	Response	response(requestInfo, 4);
	IHttpMethod	*methodGet;

	methodGet = new Get(response);

	// ACT
	methodGet->handleRequest();

	// ASSERT

	delete methodGet;
}