#ifndef IHTTPMETHOD_HPP
#define IHTTPMETHOD_HPP

# include "Request.hpp"

class Response;

class IHttpMethod {
	public:
		virtual void handleRequest() = 0;
		virtual ~IHttpMethod();
};

#endif
