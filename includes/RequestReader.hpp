#ifndef REQUESTREADER_HPP
# define REQUESTREADER_HPP

# include <string>
# include <iostream>
# include <map>
# include <cstdlib>
# include <cstring>
# include <sys/socket.h>
# include <string>
# include <typeinfo>
# include <vector>
# include <fstream>
#define CRLF			"\r\n"

class RequestReader
{

	public:
		RequestReader();
		~RequestReader();

		bool								readHttpRequest(int &fdConection);
		std::string							getMethod(void) const;
		std::string							getHttpVersion(void) const;
		std::string							getBody(void) const;
		std::string							getHeader(std::string headerName) const;
		std::string							getRequest(void) const;
		void								setBody(std::string newBody);
		void								setMethod(std::string method);
		int									getContentLength(void) const;
		std::string							getRequestedRoute(void) const;
		std::vector<std::string>			getMultipartHeaders(void) const;
		std::vector<std::string>			getMultipartValues(void) const;
		bool								isDelimiter(std::string line, std::string delimiter);
		void								readLine(int fd, std::string &line, std::string delimiter, bool &error);
		void								readLineBody(int fd, std::string &line, int contentLength, bool &error);
		std::string							intToString(int value);
	private:
		void								readRequestStartLine(void);
		void								readRequestHeader(void);
		void								readRequestBody(void);
		size_t								convertChunkSize();
		void								readRequestBodyChunked(void);
		void								readRequestBodyContentType(void);
		void								printHeaderDataStructure(void);
		void								readMultipartFormDataBody(const std::string& boundary, std::string &tempLine);

		bool									_incompleted;
		std::map<std::string, std::string> 		_headers;
		std::vector<std::string>				_multipartHeaders;
		std::vector<std::string>				_multipartValues;
		std::string								_method;
		std::string								_requestedRoute;
		std::string								_httpVersion;
		std::string								_requestBody;
		int										_fdClient;
		std::string								_request;
		bool									_errorRead;
};

#endif
