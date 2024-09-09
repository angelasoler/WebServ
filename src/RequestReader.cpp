#include "RequestReader.hpp"
#include "PrintRequestInfo.hpp"

RequestReader::RequestReader() : _errorRead(false),
	_incompleted(false),
	_headers(), _method(""),
	_requestedRoute(""),
	_httpVersion(""),
	_requestBody() {}

RequestReader::~RequestReader(void) {}

bool RequestReader::readHttpRequest(std::vector<char> &input) {

	_fullRequest = input;
	readStartLine();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_StartLine.log");
		return true;
	}
	readHeader();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_Header.log");
		return true;
	}
	readBody();
	if (_incompleted && !_errorRead) {
		PrintRequestInfo::printVectorChar(_fullRequest, "Interrupted Request", "logs/interruptedRequest_Body.log");
		return true;
	}
	PrintRequestInfo::printVectorChar(_fullRequest, "raw Request", "logs/raw_Request.log");
	return !_errorRead;
}

void RequestReader::readStartLine(void) {
	std::string line;
	readUntilCRLF(line);
	if (!line.empty()) {
		std::istringstream lineStream(line);
		if (!(lineStream >> this->_method) ||
			!(lineStream >> this->_requestedRoute) ||
			!(lineStream >> this->_httpVersion)) {
			this->_incompleted = true;
		}
	}
}

void RequestReader::readHeader(void) {
	std::string line;
	while (true) {
		readUntilCRLF(line);
		if (line == CRLF || line.empty() || _errorRead) {
			if (_errorRead) {
				PrintRequestInfo::printVectorChar(_fullRequest, "headerIncomplete_Request bytes_readed = -1 ", "logs/headerIncomplete_Request.log");
				this->_incompleted = true;
			}
			break;
		}
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			size_t lastNonCRLF = line.find_last_not_of(CRLF);
			if (lastNonCRLF != std::string::npos) {
				line = line.substr(0, lastNonCRLF + 1);
				std::string headerName = line.substr(0, colonPos);
				std::string headerValue = line.substr(colonPos + 2);
				this->_headers[headerName] = headerValue;
			}
		}
	}
	printHeaderDataStructure();
}

void RequestReader::readBody(void) {
	if (_method != "POST") return;

	long int contentLength = getContentLength();
	if (contentLength > 0) {
		readUntilSize(contentLength);
	} else {
		readUntilEOF();
	}

	if (_errorRead)
		return;

	std::string delimiter = "\r\n\r\n";
	std::vector<char>::iterator it = std::search(_fullRequest.begin(), _fullRequest.end(), delimiter.begin(), delimiter.end());

	if (it != _fullRequest.end()) {
		_rawBody.insert(_rawBody.begin(), it + delimiter.size(), _fullRequest.end());

		if (getHeader("Transfer-Encoding") == "chunked") {
			this->_requestBody = processChunkedRequestBody(_rawBody);
		} else {
			_requestBody.insert(_requestBody.begin(), _rawBody.begin(), _rawBody.end());
		}
	}
}

// CHUNK
std::vector<char> RequestReader::processChunkedRequestBody(const std::vector<char>& chunkedRequestBody) {
	std::vector<char> result;
	std::size_t i = 0;

	while (i < chunkedRequestBody.size()) {
		// Find the chunk size (hexadecimal line)
		std::string sizeStr;
		while (i < chunkedRequestBody.size() && chunkedRequestBody[i] != '\r') {
			sizeStr.push_back(chunkedRequestBody[i]);
			++i;
		}

		// Skip '\r\n'
		i += 2;

		// Convert the chunk size from hexadecimal to decimal
		std::size_t chunkSize;
		std::stringstream ss;
		ss << std::hex << sizeStr;
		ss >> chunkSize;

		if (chunkSize == 0) {
			break;
		}

		// Copy the chunk content to the result
		result.insert(result.end(), chunkedRequestBody.begin() + i, chunkedRequestBody.begin() + i + chunkSize);
		i += chunkSize;

		// Skip '\r\n' after the chunk
		i += 2;
	}

	return result;
}

// READ AND UTILS
void	 RequestReader::readUntilEOF(void)
{
	ssize_t		numberBytes = 0;
	char		buffer;

	while (true)	
	{
		// numberBytes = recv(fd, &buffer, 1, 0);
		if (numberBytes < 0) {
			if (requestCompleted(_fullRequest))
				break;
			PrintRequestInfo::printVectorChar(_fullRequest, std::string("readUntilEOF bytes_readed = " + numberBytes), "logs/readUntilEOF_Request.log");
			this->_errorRead = true;
			break ;
		}
		if (numberBytes == 0) {
			break ;
		}
		_fullRequest.push_back(buffer);
	}
}

void RequestReader::readUntilSize(long int size)
{
	ssize_t numberBytes = 0;
	std::vector<char> buffer(size);
	// numberBytes = recv(fd, &buffer[0], size, 0);
	if (numberBytes <= 0) {
		PrintRequestInfo::printVectorChar(_fullRequest, std::string("read_until_size_Request bytes_readed = " + numberBytes), "logs/readUntilSize_Request.log");
		this->_errorRead = true;
		return ;
	}
	_fullRequest.insert(_fullRequest.end(), buffer.begin(), buffer.begin() + numberBytes);
}

void	RequestReader::readUntilCRLF(std::string &segment)
{
	char		buffer = 0;
	ssize_t		numberBytes = 0;
	std::string	tempLine;

	while (true) {
		// numberBytes = recv(fd, &buffer, 1, 0);
		if (numberBytes == -1) {
			PrintRequestInfo::printVectorChar(_fullRequest, std::string("readUntilCRLF_Request bytes_readed = " + numberBytes), "logs/readUntilCRLF_Request.log");
			this->_errorRead = true;
			break;
		}
		if (numberBytes == 0) {
			PrintRequestInfo::printVectorChar(_fullRequest, std::string("readUntilCRLF_Request bytes_readed = " + numberBytes), "logs/readUntilCRLF_Request.log");
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

bool RequestReader::requestCompleted(const std::vector<char> &vec) {
	ssize_t size = vec.size();

	if (size < 4) {
		return false;
	}
	char last1 = vec[size - 4];
	char last2 = vec[size - 3];
	char last3 = vec[size - 2];
	char last4 = vec[size - 1];

	return (last1 == '\r' && last2 == '\n' && last3 == '\r' && last4 == '\n');
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
