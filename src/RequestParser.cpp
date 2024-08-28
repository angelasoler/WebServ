# include "RequestParser.hpp"
# include <sstream>

RequestParser::RequestParser(RequestInfo &info) : _headers(), _method(""), _httpVersion(""), _errorRead(false), _info(info) { }

RequestParser::~RequestParser(void) {}

bool    RequestParser::parserHttpRequest(int &fdConection)
{
	this->_fdClient = fdConection;
	// _info = info;
	// (void)info;
	parseRequestStartLine();
	parseRequestHeader();
	parseRequestBody();
	return _errorRead;
}

void parseMultipartFormDataBody(const std::string& boundary, std::string &tempLine)
{
	size_t boundaryPos = tempLine.find(boundary);
	if (boundaryPos == std::string::npos)
	{
		return ;
	}
	size_t contentStart = tempLine.find("\r\n\r\n", boundaryPos);
	if (contentStart == std::string::npos)
	{
		return ;
	}
	contentStart += 4;
	if (tempLine[contentStart] == '\r')
	{
		++contentStart;
	}
	size_t contentEnd = tempLine.find(boundary, contentStart);
	if (contentEnd == std::string::npos)
	{
		tempLine =  tempLine.substr(contentStart, contentEnd - contentStart);
		return ;
	}
	while (contentEnd > contentStart && (tempLine[contentEnd - 1] == '\r' || tempLine[contentEnd - 1] == '\n' || tempLine[contentEnd - 1] == '-'))
	{
		--contentEnd;
	}
	tempLine = tempLine.substr(contentStart, contentEnd - contentStart);
}

size_t  RequestParser::convertChunkSize(void)
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

void RequestParser::parseRequestBodyChunked()
{
	std::size_t		length = 0;
	std::string        tempLine;

	std::size_t     chunkSize = convertChunkSize();

	while (chunkSize > 0)
	{
		readLine(this->_fdClient, tempLine, CRLF, this->_errorRead);
		_info.body += tempLine + "\n";
		this->_request += tempLine + "\n";

		length += chunkSize;
		readLine(this->_fdClient, tempLine, CRLF, this->_errorRead);
		chunkSize = convertChunkSize();
	}
	this->_headers["Content-Length"] = intToString(length);
}

void RequestParser::parseRequestBodyContentType(void)
{
	std::string tempLine;
	size_t pos;

	readLineBody(this->_fdClient, tempLine, getContentLength(), this->_errorRead);
	setFileName(tempLine);
	pos = getHeader("Content-Type").find("boundary=", 0);
	if (pos != std::string::npos)
	{
		std::string boundary = getHeader("Content-Type").substr(pos + 9);
		parseMultipartFormDataBody(boundary, tempLine);
	}
	_info.body += tempLine;
	this->_request += tempLine + "\n";

}
void RequestParser::parseRequestBody(void)
{
	if (getHeader("Transfer-Encoding") == "chunked")
	{
		parseRequestBodyChunked();
	}
	else if (!getHeader("Content-Type").empty() && getHeader("Content-Type").find("multipart/form-data") != std::string::npos)
	{
		parseRequestBodyContentType();
	}
	else if (!getHeader("Content-Length").empty())
	{
			std::string tempLine;

		readLineBody(this->_fdClient, tempLine, getContentLength(), this->_errorRead);
		_info.body += tempLine;
		this->_request += tempLine + "\n";
	}
}

void 	RequestParser::parseRequestHeader(void)
{
	std::string line;

	while (true)
	{
		readLine(this->_fdClient, line, CRLF, this->_errorRead);
		if (line == CRLF || line.empty())
		{
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
		this->_request += line + "\n";
	}
}

void	RequestParser::parseRequestStartLine(void)
{
	std::string line;

	readLine(_fdClient, line, CRLF, this->_errorRead);
	if (!line.empty())
	{
		std::istringstream lineStream(line);
		lineStream >> this->_method >> _info.requestedRoute >> this->_httpVersion;
		this->_request += line +  "\n";
	}
}

std::string RequestParser::getMethod(void) const
{
	return this->_method;
}

std::string RequestParser::getHttpVersion(void) const
{
	return this->_httpVersion;
}

std::string RequestParser::getBody(void) const
{
	return _info.body;
}

std::string RequestParser::getHeader(std::string headerName) const
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

std::string RequestParser::getFileName(void) const
{
	return this->_fileName;
}


std::string RequestParser::getRequest(void) const
{
	return this->_request;
}

void RequestParser::setBody(std::string newBody)
{
	_info.body = newBody;
}

std::string RequestParser::getFileExec(void) const
{
	return this->_fileExec;
}

void RequestParser::setFileExec(std::string fileExec)
{
	this->_fileExec = fileExec;
}

void RequestParser::setFileName(std::string line)
{

	if (line.find("filename=\"") != std::string::npos)
	{
		size_t filenameEndPos = line.find("\"", line.find("filename=\"") + 10);
		if (filenameEndPos != std::string::npos)
		{
			_fileName = line.substr(line.find("filename=\"") + 10, filenameEndPos - (line.find("filename=\"") + 10));
		}
	}

}

int RequestParser::getContentLength() const
{
	std::string contentLengthStr = getHeader("Content-Length");

	if (!contentLengthStr.empty())
	{
		return atol(contentLengthStr.c_str());
	}
	return 0;
}

void RequestParser::setMethod(std::string method)
{
	this->_method = method;
}


bool  RequestParser::isDelimiter(std::string line, std::string delimiter)
{
	return (line.rfind(delimiter) != std::string::npos);
}



void	 RequestParser::readLine(int fd, std::string &line, std::string delimiter, bool &error)
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
			std::cerr << "readLine: " << "std::strerror(errno) " << std::endl;
			break;
		}
		if (numberBytes == 0)
		{
			error = true;
			std::cerr << "readLine: " << "std::strerror(errno) " << std::endl;
			break ;
		}
		tempLine += buffer;
		if (isDelimiter(tempLine, delimiter))
			break ;
	}
	line = tempLine;
	if (isDelimiter(tempLine, delimiter))
		line.resize(line.rfind(delimiter));
}

void	 RequestParser::readLineBody(int fd, std::string &line, int contentLength, bool &error)
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
			std::cerr << "readLineBody: " << "std::strerror(errno) " << std::endl;
			break;
		}
		if (numberBytes == 0)
		{
			error = true;
			std::cerr << "readLineBody: " << "std::strerror(errno) " << std::endl;
			break ;
		}
		contentLength -= numberBytes;
		line.append(buffer, numberBytes);
		memset(buffer, 0, 20);
	}
}

std::string RequestParser::intToString(int value)
{
	std::stringstream ss;
	ss << value;
	return ss.str();
}
