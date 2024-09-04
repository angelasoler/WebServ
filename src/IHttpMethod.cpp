# include "IHttpMethod.hpp"
# include "Response.hpp"
# include "CGIServer.hpp"

IHttpMethod::~IHttpMethod() {}

int	IHttpMethod::responseCGI(Response &response) {
	CGIServer	cgi(response.requestInfo);

	cgi.setEnv();
	cgi.executeScript();

	response.setBody(cgi.CGIReturn.body);
	return(cgi.CGIReturn.code);
}
