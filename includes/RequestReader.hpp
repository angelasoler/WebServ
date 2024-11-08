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

class RequestReader
{

	public:
		RequestReader();
		~RequestReader();

		bool								readHttpRequest(std::vector<char> &input);

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

		// DEBUG
		void								printHeaderDataStructure(void);

		// HEADER VARS
		std::map<std::string, std::string> 		_headers;
		std::string								_method;
		std::string								_requestedRoute;
		std::string								_httpVersion;

		// REQUEST VARS
		std::vector<char>						_fullRequest;
		std::vector<char>						_requestBody;
		std::vector<char>						_rawBody;
};

#endif
