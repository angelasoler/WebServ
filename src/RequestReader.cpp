#include "RequestReader.hpp"
#include "PrintRequestInfo.hpp"

RequestReader::RequestReader() : _headers(), _method(""),
	_requestedRoute(""),
	_httpVersion("") {}

RequestReader::~RequestReader(void) {}

bool RequestReader::readHttpRequest(std::vector<char> &input) {
	_fullRequest = input;

	PrintRequestInfo::printVectorChar(_fullRequest, "fullRequest", "logs/fullRequest.log");
	std::string requestStr(input.begin(), input.end());

	std::istringstream stream(requestStr);
	std::string line;

	if (std::getline(stream, line)) {
		std::istringstream requestLine(line);
		requestLine >> _method;
		requestLine >> _requestedRoute;
		requestLine >> _httpVersion;
	}

	bool bodyFound = false;
	while (std::getline(stream, line)) {
		if (line == "\r") {
			bodyFound = true;
			break;
		}

		size_t separator = line.find(": ");
		if (separator != std::string::npos) {
			std::string headerName = line.substr(0, separator);
			std::string headerValue = line.substr(separator + 2);
			headerValue.erase(headerValue.find_last_not_of("\r\n") + 1);
			_headers[headerName] = headerValue;
		}
	}

	if (bodyFound) {
		std::string delimiter = "\r\n\r\n";
		std::vector<char>::iterator it = std::search(_fullRequest.begin(), _fullRequest.end(), delimiter.begin(), delimiter.end());

		if (it != _fullRequest.end())
		{
			_rawBody.insert(_rawBody.begin(), it + delimiter.size(), _fullRequest.end());

			if (getHeader("Transfer-Encoding") == "chunked") {
				this->_requestBody = processChunkedRequestBody(_rawBody);
			} else {
				_requestBody.insert(_requestBody.begin(), _rawBody.begin(), _rawBody.end());
			}
		}
	}
	PrintRequestInfo::printVectorChar(_requestBody, "Request_body", "logs/Request_body.log");
	PrintRequestInfo::printVectorChar(_fullRequest, "raw Request", "logs/raw_Request.log");
	return (false);
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
