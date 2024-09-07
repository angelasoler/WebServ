#include "RequestReader.hpp"

RequestReader::RequestReader(void) : _errorRead(false),
_incompleted(false),
_readRawBody(false),
_headers(), _method(""),
_requestedRoute(""),
_httpVersion(""),
_requestBody() {}

RequestReader::~RequestReader(void) {}

bool    RequestReader::readHttpRequest(int &fdConection)
{
	this->_fdClient = fdConection;
	readStartLine();
	if (_incompleted && !_errorRead)
		return true;
	readHeader();
	if (_incompleted && !_errorRead)
		return true;
	readBody();
	if (_incompleted && !_errorRead)
		return true;
	if (_errorRead)
		return false;
	return true;
}

void RequestReader::readStartLine(void)
{
	std::string line;

	readRequestSegment(_fdClient, line, CRLF);
	if (!line.empty())
	{
		std::istringstream lineStream(line);
		if (!(lineStream >> this->_method)) {
			this->_incompleted = true;
			return;
		}
		if (!(lineStream >> this->_requestedRoute)) {
			this->_incompleted = true;
			return;
		}
		if (!(lineStream >> this->_httpVersion)) {
			this->_incompleted = true;
			return;
		}
	}
}

void 	RequestReader::readHeader(void)
{
	std::string line;

	while (true)
	{
		readRequestSegment(_fdClient, line, CRLF);
		if (line == CRLF || line.empty() || _errorRead) {
			if (_errorRead)
				this->_incompleted = true;
			break;
		}
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos)
		{
			size_t lastNonCRLF = line.find_last_not_of(CRLF);
			if (lastNonCRLF != std::string::npos)
			{
				line = line.substr(0, lastNonCRLF + 1);
				std::string headerName = line.substr(0, colonPos);
				std::string headerValue = line.substr(colonPos + 2);
				this->_headers[headerName] = headerValue;

			}
		}
	}
	printHeaderDataStructure();
}

void	 RequestReader::readUntilLimit(int fd, long int contentLength)
{
	ssize_t		numberBytes;
	char		buffer[20] = {0};

	while (true)	
	{
		if (!contentLength)
			break;
		numberBytes = recv(fd, buffer, 20, 0);
		if (numberBytes == -1 || numberBytes == 0) {
			this->_errorRead = true;
			break ;
		}
		contentLength -= numberBytes;
		_rawBody.insert(_rawBody.end(), buffer, buffer + numberBytes);
		_requestBody.insert(_requestBody.end(), buffer, buffer + numberBytes);
		this->_fullRequest.push_back(buffer[0]);
		memset(buffer, 0, 20);
	}
}

# include "PrintRequestInfo.hpp"
void RequestReader::readBody(void)
{
	bool	isChunked = (getHeader("Transfer-Encoding") == "chunked");
	bool	isMultipart = (!getHeader("Content-Type").empty() && getHeader("Content-Type").find("multipart/form-data") != std::string::npos);
	_readRawBody = true;
	std::string tempLine;

	if (isChunked)
	{
		if (isMultipart)
			readRequestBodyChunkedMultipart();
		else
			readRequestBodyChunked();
		return ;
	}
	else
		readUntilLimit(this->_fdClient, getContentLength());

	PrintRequestInfo::printVectorChar(_requestBody, "Read Body", "logs/readBody.log");
	if (isMultipart)
	{
		readRequestBodyMultipart();
	}
}

// Chunked
size_t  RequestReader::readChunkSize(void)
{
	std::string line;
	std::string chunkSizeLine;
	std::size_t chunkSize = 0;

	readRequestSegment(this->_fdClient, line, CRLF);
	if (line == "")
		return 0;
	chunkSizeLine = line.substr(0,  line.find(" "));

	std::stringstream ss;
	ss << std::hex << chunkSizeLine;
	ss >> chunkSize;
	return chunkSize;
}

void RequestReader::readRequestBodyChunkedMultipart(void)
{
	std::size_t	length = 0;
	std::string	tempLine;
	std::size_t	chunkSize = readChunkSize();

	while (chunkSize > 0)
	{
		readUntilLimit(this->_fdClient, chunkSize);
		length += chunkSize;
		chunkSize = readChunkSize();
	}
	this->_headers["Content-Length"] = intToString(length);
}

void RequestReader::readRequestBodyChunked()
{
	std::size_t	length = 0;
	std::string	tempLine;
	std::size_t	chunkSize = readChunkSize();

	while (chunkSize > 0)
	{
		readRequestSegment(this->_fdClient, tempLine, CRLF);
		_requestBody.insert(_requestBody.end(), tempLine.begin(), tempLine.end());
		length += chunkSize;
		readRequestSegment(this->_fdClient, tempLine, CRLF);
		chunkSize = readChunkSize();
	}
	this->_headers["Content-Length"] = intToString(length);
}

// Multipart
void RequestReader::readRequestBodyMultipart(void)
{
	std::string boundary;
	size_t pos;

	pos = getHeader("Content-Type").find("boundary=", 0);
	if (pos != std::string::npos)
		boundary = getHeader("Content-Type").substr(pos + 9);
	if (pos != std::string::npos)
	{
		readMultipartInfo(boundary, _requestBody);
	}
}

void RequestReader::readMultipartInfo(const std::string& boundary, std::vector<char> &tempLine)
{
	size_t boundaryPos = 0;
	size_t contentStart = 0;

	while ((boundaryPos = std::search(tempLine.begin() + boundaryPos, tempLine.end(), boundary.begin(), boundary.end()) - tempLine.begin()) != tempLine.size())
	{
		// Encontrar o início do conteúdo depois do cabeçalho
		std::vector<char>::iterator contentStartIt = std::search(tempLine.begin() + boundaryPos, tempLine.end(), "\r\n\r\n", "\r\n\r\n" + 4);
		if (contentStartIt == tempLine.end())
		{
			return;
		}

		contentStart = contentStartIt - tempLine.begin() + 4;

		// Extrair o cabeçalho multipart
		std::string multipartHeader(tempLine.begin() + boundaryPos + boundary.size(), contentStartIt);
		_multipartHeaders.push_back(multipartHeader);

		// Encontrar o fim do conteúdo antes do próximo boundary
		size_t contentEnd = std::search(tempLine.begin() + contentStart, tempLine.end(), boundary.begin(), boundary.end()) - tempLine.begin();
		if (contentEnd == tempLine.size())
		{
			contentEnd = tempLine.size();
		}

		// Remover possíveis '\r', '\n' e '-' do final do conteúdo
		while (contentEnd > contentStart && (tempLine[contentEnd - 1] == '\r' || tempLine[contentEnd - 1] == '\n' || tempLine[contentEnd - 1] == '-'))
		{
			--contentEnd;
		}

		// Extrair o valor do multipart
		std::string multipartValue(tempLine.begin() + contentStart, tempLine.begin() + contentEnd);
		_multipartValues.push_back(multipartValue);

		// Ajustar boundaryPos para continuar a busca
		boundaryPos = contentEnd;
	}
}

// READ SEGMENTS
void	 RequestReader::readRequestSegment(int fd, std::string &segment, std::string delimiter)
{
	int			buffer = 0;
	ssize_t		numberBytes;
	std::string tempLine;

	while (true) {
		numberBytes = recv(fd, &buffer, 1, 0);
		if (numberBytes == -1 || numberBytes == 0) {
			this->_errorRead = true;
			break ;
		}
		tempLine += buffer;
		this->_fullRequest.push_back(buffer);
		if (this->_readRawBody)
			this->_rawBody.push_back(buffer);
		if (isDelimiter(tempLine, delimiter))
			break ;
	}
	segment = tempLine;
	if (isDelimiter(tempLine, delimiter))
		segment.resize(segment.rfind(delimiter));
}

std::string RequestReader::intToString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}

bool	RequestReader::isDelimiter(std::string line, std::string delimiter)
{
	return (line.rfind(delimiter) != std::string::npos);
}


// GETTERS

std::string RequestReader::getMethod(void) const
{
	return this->_method;
}

std::string RequestReader::getRequestedRoute(void) const
{
	return this->_requestedRoute;
}

std::string RequestReader::getHttpVersion(void) const
{
	return this->_httpVersion;
}

std::string RequestReader::getBody(void) const
{
	return std::string(_requestBody.begin(), _requestBody.end());
}

std::vector<char>	RequestReader::getRawBody(void) const
{
	return this->_rawBody;
}

std::string RequestReader::getHeader(std::string headerName) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(headerName);

	if (it != this->_headers.end())
	{
		return it->second;
	}
	else
	{
		return "";
	}
}

std::string RequestReader::getFullRequest(void) const
{
	return std::string(this->_fullRequest.begin(), this->_fullRequest.end());
}

long int RequestReader::getContentLength() const
{
	std::string contentLengthStr = getHeader("Content-Length");

	if (!contentLengthStr.empty())
	{
		return atol(contentLengthStr.c_str());
	}
	return 0;
}

std::vector<std::string>	RequestReader::getMultipartHeaders(void) const
{
	return _multipartHeaders;
}

std::vector<std::string>	RequestReader::getMultipartValues(void) const
{
	return _multipartValues;
}

// DEBUG
void RequestReader::printHeaderDataStructure(void)
{
	std::map<std::string, std::string>::iterator headerIt;
	std::ofstream parsedRequest("logs/parsedHeader.log");

	if (!parsedRequest.is_open()) {
		return;
	}
	parsedRequest << "\t\t === parsed header ===" << std::endl;
	for (headerIt = _headers.begin(); headerIt != _headers.end(); ++headerIt) {
		parsedRequest << headerIt->first << std::endl;
		parsedRequest << "\t" << headerIt->second << std::endl;
	}
	parsedRequest << "\t\t === \t\t ===" << std::endl;
	parsedRequest.close();
}
