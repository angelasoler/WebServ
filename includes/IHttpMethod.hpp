#ifndef IHTTPMETHOD_HPP
#define IHTTPMETHOD_HPP

# include "Request.hpp"

class Response;

class IHttpMethod {
	public:
		virtual void buildBody(void) = 0;
		virtual int handleRequest() = 0;
		virtual ~IHttpMethod();
		int		responseCGI(Response &response);
};

#endif
