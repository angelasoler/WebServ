# include "IHttpMethod.hpp"

IHttpMethod::~IHttpMethod() {}

// int	IHttpMethod::responseCGI(Response &response) {
// 	CGIServer	cgi(response.requestInfo);

// 	cgi.setEnv();
// 	cgi.executeScript();

// 	response.setBody(cgi.CGIReturn.body);
// 	return(cgi.CGIReturn.code);
// }
