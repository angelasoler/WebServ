#ifndef REQUESTPARSER_HPP
# define REQUESTPARSER_HPP

# include <string>
# include <iostream>
# include <map>
# include <cstdlib>
# include <cstring>
# include <sys/socket.h>
# include <string>
# include <typeinfo>
# include <vector>
# include "Request.hpp"
#define CRLF			"\r\n"

class RequestParser
{

	public:
		RequestParser (RequestInfo &info);
		~RequestParser();

		bool								parserHttpRequest(int &fdConection);
		std::string							getMethod(void) const;
		std::string							getHttpVersion(void) const;
		std::string							getBody(void) const;
		std::string							getHeader(std::string headerName) const;
		std::string							getFileExec(void) const;
		std::string							getFileName(void) const;
		std::string							getRequest(void) const;
		void								setBody(std::string newBody);
		void								setFileExec(std::string newFileExec);
		void								setMethod(std::string method);
		int									getContentLength(void) const;

		bool								isDelimiter(std::string line, std::string delimiter);
		void								readLine(int fd, std::string &line, std::string delimiter, bool &error);
		void								readLineBody(int fd, std::string &line, int contentLength, bool &error);
		std::string							intToString(int value);
	private:
		void								parseRequestStartLine(void);
		void								parseRequestHeader(void);
		void								parseRequestBody(void);
		void								setFileName(std::string file);
		size_t								convertChunkSize();
		void								parseRequestBodyChunked(void);
		void								parseRequestBodyContentType(void);



		std::map<std::string, std::string> 	_headers;
		std::string							_method;
		std::string							_httpVersion;
		std::string							_fileExec;
		std::string							_fileName;
		int									_fdClient;
		std::string							_request;
		bool								_errorRead;
		RequestInfo							&_info;
};

#endif
