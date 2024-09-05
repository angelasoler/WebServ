#include "RequestReader.hpp"
# include <sstream>

RequestReader::RequestReader(void) : _errorRead(false), _incompleted(false), _readRawBody(false), _headers(), _method(""), _requestedRoute(""), _httpVersion(""), _requestBody() {}

RequestReader::~RequestReader(void) {}

bool    RequestReader::readHttpRequest(int &fdConection)
{
	this->_fdClient = fdConection;
	readRequestStartLine();
	if (_incompleted && !_errorRead)
		return true;
	readRequestHeader();
	if (_incompleted && !_errorRead)
		return true;
	readRequestBody();
	if (_incompleted && !_errorRead)
		return true;
	if (_errorRead)
		return false;
	return true;
}


// READ START LINE
void RequestReader::readRequestStartLine(void)
{
	std::string line;

	readLine(_fdClient, line, CRLF, this->_errorRead);
	if (!line.empty())
	{
		this->_fullRequest += line +  "\n";
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

// READ REQUEST HEADER
void 	RequestReader::readRequestHeader(void)
{
	std::string line;

	while (true)
	{
		readLine(this->_fdClient, line, CRLF, this->_errorRead);
		if (line == CRLF || line.empty() || _errorRead)
		{
			if (_errorRead)
				this->_incompleted = true;
			break;
		}
		else
		{
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
		this->_fullRequest += line + "\n";
	}
	printHeaderDataStructure();
}

// READ REQUEST BODY
void RequestReader::readRequestBody(void)
{
	_readRawBody = true;
	if (getHeader("Transfer-Encoding") == "chunked")
	{
		readRequestBodyChunked();
	}
	else if (!getHeader("Content-Type").empty() && getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
	{
		readRequestBodyMultipart();
	}
	else if (!getHeader("Content-Length").empty())
	{
			std::string tempLine;

		readLineBody(this->_fdClient, tempLine, getContentLength(), this->_errorRead);
		_requestBody.insert(_requestBody.end(), tempLine.begin(), tempLine.end());
		this->_fullRequest += tempLine + "\n";
	}
}

void RequestReader::readRequestBodyChunked()
{
	std::size_t	length = 0;
	std::string	tempLine;
	std::size_t	chunkSize = readChunkSize();

	while (chunkSize > 0)
	{
		readLine(this->_fdClient, tempLine, CRLF, this->_errorRead);
		_requestBody.insert(_requestBody.end(), tempLine.begin(), tempLine.end());
		this->_fullRequest += tempLine;

		length += chunkSize;
		readLine(this->_fdClient, tempLine, CRLF, this->_errorRead);
		chunkSize = readChunkSize();
	}
	this->_headers["Content-Length"] = intToString(length);
}

size_t  RequestReader::readChunkSize(void)
{
	std::string line;
	std::string chunkSizeLine;

	readLine(this->_fdClient, line, CRLF, this->_errorRead);
	std::size_t chunkSize = 0;

	if (line == "")
		return 0;
	chunkSizeLine = line.substr(0,  line.find(" "));

	std::stringstream ss;
	ss << std::hex << chunkSizeLine;
	ss >> chunkSize;
	return chunkSize;
}

void RequestReader::readRequestBodyMultipart(void)
{
	std::string tempLine;
	size_t pos;

	readLineBody(this->_fdClient, tempLine, getContentLength(), this->_errorRead);
	pos = getHeader("Content-Type").find("boundary=", 0);
	if (pos != std::string::npos)
	{
		std::string boundary = getHeader("Content-Type").substr(pos + 9);
		readMultipartInfo(boundary, tempLine);
	}
	_requestBody.insert(_requestBody.end(), tempLine.begin(), tempLine.end());
	this->_fullRequest += tempLine + "\n";

}

void RequestReader::readMultipartInfo(const std::string& boundary, std::string &tempLine)
{
	size_t boundaryPos = 0;
	size_t contentStart = 0;
	std::string multipartHeader;

	while ((boundaryPos = tempLine.find(boundary, boundaryPos)) != std::string::npos)
	{
		contentStart = tempLine.find("\r\n\r\n", boundaryPos);
		if (contentStart == std::string::npos)
		{
			return;
		}

		multipartHeader = tempLine.substr(boundaryPos + boundary.size(), contentStart - (boundaryPos + boundary.size()));
		// std::cerr << "Multipart header:\n\t" << multipartHeader << std::endl;
		_multipartHeaders.push_back(multipartHeader);

		contentStart += 4; // Pular "\r\n\r\n"
		if (contentStart >= tempLine.size())
		{
			return;
		}

		size_t contentEnd = tempLine.find(boundary, contentStart);
		if (contentEnd == std::string::npos)
		{
			contentEnd = tempLine.size();
		}

		// Remover possíveis '\r', '\n' e '-' do final do conteúdo
		while (contentEnd > contentStart && (tempLine[contentEnd - 1] == '\r' || tempLine[contentEnd - 1] == '\n' || tempLine[contentEnd - 1] == '-'))
		{
			--contentEnd;
		}

		std::string multipartValue = tempLine.substr(contentStart, contentEnd - contentStart);
		// std::cerr << "Multipart body:\n\t" << multipartValues << std::endl;
		_multipartValues.push_back(multipartValue);

		// Atualizar tempLine para o conteúdo restante após o bodypart
		tempLine = tempLine.substr(contentEnd);

		// Ajustar boundaryPos para a próxima ocorrência do boundary
		boundaryPos = 0;
	}
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
	return this->_fullRequest;
}

int RequestReader::getContentLength() const
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


// READ LINE UTILS
void	 RequestReader::readLine(int fd, std::string &line, std::string delimiter, bool &error)
{
	char		buffer[2] = {0};
	ssize_t		numberBytes;
	std::string tempLine;

	while (true)
	{
		numberBytes = recv(fd, buffer, 1, 0);
		if (numberBytes == -1)
		{
			error = true;
			// std::cerr << "readLine: " << "bytes readed: " << numberBytes << std::endl;
			break;
		}
		if (numberBytes == 0)
		{
			error = true;
			// std::cerr << "readLine: " << "bytes readed: " << numberBytes << std::endl;
			break ;
		}
		tempLine += buffer;
		if (_readRawBody)
			_rawBody.push_back(buffer[0]);
		if (isDelimiter(tempLine, delimiter))
			break ;
	}
	line = tempLine;
	if (isDelimiter(tempLine, delimiter))
		line.resize(line.rfind(delimiter));
}

void	 RequestReader::readLineBody(int fd, std::string &line, int contentLength, bool &error)
{
	ssize_t		numberBytes;
	char		buffer[20] = {0};

	while (true)	
	{
		if (!contentLength)
			break;
		numberBytes = recv(fd, buffer, 20, 0);
		if (numberBytes == -1)
		{
			error = true;
			// std::cerr << "readLineBody: " << "bytes readed: " << numberBytes << std::endl;
			break;
		}
		if (numberBytes == 0)
		{
			error = true;
			// std::cerr << "readLineBody: " << "bytes readed: " << numberBytes << std::endl;
			break ;
		}
		contentLength -= numberBytes;
		if (_readRawBody) {
			_rawBody.insert(_rawBody.end(), buffer, buffer + numberBytes);
		}
		line.append(buffer, numberBytes);
		memset(buffer, 0, 20);
	}
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
