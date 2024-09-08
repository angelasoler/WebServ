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
# include <sstream>
# include <algorithm>
# define CRLF	"\r\n"
# define READ_BUFFER_SIZE 1024

class RequestReader
{

	public:
		RequestReader();
		~RequestReader();

		bool								readHttpRequest(int &fdConection);

		// GETTERS
		std::string							getMethod(void) const;
		std::string							getHttpVersion(void) const;
		std::string							getBody(void) const;
		std::vector<char>					getRawBody(void) const;
		std::string							getHeader(std::string headerName) const;
		std::string							getFullRequest(void) const;
		long int							getContentLength(void) const;
		std::string							getRequestedRoute(void) const;
	private:

		// READ START LINE
		void								readStartLine(void);

		// READ REQUEST HEADER
		void								readHeader(void);

		// READ REQUEST BODY
		void								readBody(void);
		std::vector<char>					processChunkedRequestBody(const std::vector<char>& chunkedRequestBody);
		void								readRequestBodyChunked(void);
		void								readRequestBodyChunkedMultipart(void);
		size_t								readChunkSize();

		// READLINE AND UTILS
		void	 							readUntilEOF(int fd);
		void								readUntilSize(int fd, long int size);
		void								readUntilCRLF(int fd, std::string &segment);
		std::string							intToString(int value);
		bool								isDelimiter(std::string line, std::string delimiter);

		// DEBUG
		void								printHeaderDataStructure(void);

		// STATUS VARS
		bool									_errorRead;
		bool									_incompleted;

		// REQUEST VARS
		std::map<std::string, std::string> 		_headers;
		std::string								_method;
		std::string								_requestedRoute;
		std::string								_httpVersion;
		std::vector<char>						_requestBody;
		int										_fdClient;
		std::vector<char>						_fullRequest;
		std::vector<char>						_rawBody;
};

#endif
