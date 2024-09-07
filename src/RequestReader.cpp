#include "RequestReader.hpp"
# include "PrintRequestInfo.hpp"

RequestReader::RequestReader(void) : _errorRead(false),
_incompleted(false),
_headers(), _method(""),
_requestedRoute(""),
_httpVersion(""),
_requestBody() {}

RequestReader::~RequestReader(void) {}

bool    RequestReader::readHttpRequest(int &fdConection)
{
	this->_fdClient = fdConection;
	readStartLine();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_1.log");
		return true;
	}
	readHeader();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_2.log");
		return true;
	}
	readBody();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_3.log");
		return true;
	}
	PrintRequestInfo::printVectorChar(_fullRequest, "raw Request", "logs/rawRequest.log");
	if (_errorRead)
		return false;
	return true;
}

void RequestReader::readStartLine(void)
{
	std::string line;

	readUntilCRLF(_fdClient, line);
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
		readUntilCRLF(_fdClient, line);
		if (line == CRLF || line.empty() || _errorRead) {
			if (_errorRead) {
				PrintRequestInfo::printVectorChar(_fullRequest, "headerIncomplete_Request bytes_readed = -1 ", "logs/headerIncomplete_Request.log");
				this->_incompleted = true;
			}
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

void RequestReader::readBody(void)
{
	if (_method != "POST")
		return ;

	long int contentLength = getContentLength();
	if (contentLength > 0)
		readUntilSize(this->_fdClient, contentLength);
	else
		readUntilEOF(this->_fdClient);

	if (getHeader("Transfer-Encoding") == "chunked")
	{
		this->_requestBody = processChunkedRequestBody(_rawBody);
	}
	else
		_requestBody.insert(_requestBody.begin(), _rawBody.begin(), _rawBody.end());
}

// Chunked
std::vector<char> RequestReader::processChunkedRequestBody(const std::vector<char>& chunkedRequestBody)
{
	std::vector<char> result;
	std::size_t i = 0;
	
	while (i < chunkedRequestBody.size()) {
		// Encontrar o tamanho do chunk (linha em hexadecimal)
		std::string sizeStr;
		while (i < chunkedRequestBody.size() && chunkedRequestBody[i] != '\r') {
			sizeStr.push_back(chunkedRequestBody[i]);
			++i;
		}
		
		// Saltar '\r\n'
		i += 2;

		// Converter o tamanho do chunk de hexadecimal para decimal
		std::size_t chunkSize;
		std::stringstream ss;
		ss << std::hex << sizeStr;
		ss >> chunkSize;

		if (chunkSize == 0) {
			break;
		}

		// Copiar o conteúdo do chunk para o resultado
		// result.insert(result.end(), )
		for (std::size_t j = 0; j < chunkSize && i < chunkedRequestBody.size(); ++j, ++i) {
			result.push_back(chunkedRequestBody[i]);
		}

		// Saltar '\r\n' após o chunk
		i += 2;
	}

	return result;
}

// READ SEGMENTS
void	 RequestReader::readUntilEOF(int fd)
{
	ssize_t		numberBytes;
	char		buffer[READ_BUFFER_SIZE] = {0};

	while (true)	
	{
		numberBytes = recv(fd, buffer, READ_BUFFER_SIZE, 0);
		if (numberBytes == -1) {
			this->_errorRead = true;
			PrintRequestInfo::printVectorChar(_fullRequest, "readuntilEOF_Request bytes_readed = -1 ", "logs/readuntilEOF_Request.log");
			break ;
		}
		if (numberBytes == 0) {
			PrintRequestInfo::printVectorChar(_fullRequest, "readuntilEOF_Request bytes_readed = 0 ", "logs/readuntilEOF_Request.log");
			break ;
		}
		_rawBody.insert(_rawBody.end(), buffer, buffer + numberBytes);
		_fullRequest.insert(_fullRequest.end(), buffer, buffer + numberBytes);
		if (numberBytes < READ_BUFFER_SIZE)
			return;
	}
}

void RequestReader::readUntilSize(int fd, long int size)
{
	ssize_t numberBytes;
	std::vector<char> buffer(size);
	numberBytes = recv(fd, &buffer[0], size, 0);
	if (numberBytes <= 0) {
		PrintRequestInfo::printVectorChar(_fullRequest, std::string("read_until_size_Request bytes_readed = " + numberBytes), "logs/readUntilSize_Request.log");
		this->_errorRead = true;
		return ;
	}
	_rawBody.insert(_rawBody.end(), buffer.begin(), buffer.begin() + numberBytes);
	_fullRequest.insert(_fullRequest.end(), buffer.begin(), buffer.begin() + numberBytes);
}

void	RequestReader::readUntilCRLF(int fd, std::string &segment)
{
	char		buffer = 0;
	ssize_t		numberBytes = 0;
	std::string	tempLine;

	while (true) {
		numberBytes = recv(fd, &buffer, 1, 0);
		if (numberBytes == -1 || numberBytes == 0) {
			PrintRequestInfo::printVectorChar(_fullRequest, "readuntilCRLF_Request bytes_readed = -1 ", "logs/readuntilCRLF_Request.log");
			this->_errorRead = true;
			break;
		}
		tempLine += buffer;
		this->_fullRequest.push_back(buffer);
		if (isDelimiter(tempLine, CRLF))
			break;
	}
	segment = tempLine;
	if (isDelimiter(tempLine, CRLF))
		segment.resize(segment.rfind(CRLF));
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

// DEBUG
void RequestReader::printHeaderDataStructure(void)
{
	std::map<std::string, std::string>::iterator headerIt;
	std::ofstream parsedRequest("logs/parsedHeader.log", std::ios_base::app);

	if (!parsedRequest.is_open()) {
		return;
	}
	parsedRequest << "\t\t === PARSED HEADER ===\n" << std::endl;
	for (headerIt = _headers.begin(); headerIt != _headers.end(); ++headerIt) {
		parsedRequest << headerIt->first << ": ";
		parsedRequest << headerIt->second << std::endl;
	}
	parsedRequest << "\n\t\t === END OF HEADER ===" << std::endl << "\n";
	parsedRequest.close();
}
