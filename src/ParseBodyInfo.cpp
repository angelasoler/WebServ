#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

static void parseRequestBodyMultipart(RequestInfo &info);
static void parseMultipartInfo(const std::string& boundary, std::vector<char> &tempLine, RequestInfo &info);

void ParseBodyInfo::parseBodyInfo(RequestInfo &info)
{	
	if (info.action != UPLOAD)
		return;
	if (info.contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		parseUrlEncodedValues(info);
	if (info.contentType.find("multipart/form-data") != std::string::npos)
	{
		parseRequestBodyMultipart(info);
	}	
}

void parseUrlEncodedValues(RequestInfo &info)
{
	size_t start = 0;
	size_t end = info.body.find('&');

	while (end != std::string::npos)
	{
		std::string pair = info.body.substr(start, end - start);
		size_t separator = pair.find('=');
		if (separator != std::string::npos)
		{
			std::string key = pair.substr(0, separator);
			std::string value = pair.substr(separator + 1);

			key = trim(key);
			value = trim(value);

			if (!key.empty()) {
				info.urlencodedValues[key] = value;
			}
			else {
				// std::cerr << "Error: Encountered an empty key in the request body." << std::endl;
			}
		}
		else {
			// std::cerr << "Error: Malformed key-value pair '" << pair << "' in the request body." << std::endl;
		}

		start = end + 1;
		end = info.body.find('&', start);
	}

	std::string pair = info.body.substr(start);
	size_t separator = pair.find('=');
	if (separator != std::string::npos)
	{
		std::string key = pair.substr(0, separator);
		std::string value = pair.substr(separator + 1);

		key = trim(key);
		value = trim(value);

		if (!key.empty()) {
			info.urlencodedValues[key] = value;
		}
		else {
			// std::cerr << "Error: Encountered an empty key in the request body." << std::endl;
		}
	}
	else if (!pair.empty()) {
		// std::cerr << "Error: Malformed key-value pair '" << pair << "' in the request body." << std::endl;
	}
}

std::string trim(const std::string& str)
{
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos)
		return ""; // No content
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, last - first + 1);
}

static void parseRequestBodyMultipart(RequestInfo &info)
{
	std::string boundary;
	size_t pos;

	pos = info.contentType.find("boundary=", 0);
	if (pos != std::string::npos)
		boundary = info.contentType.substr(pos + 9);
	if (pos != std::string::npos)
	{
		parseMultipartInfo(boundary, info.rawBody, info);
	}
}

static void parseMultipartInfo(const std::string& boundary, std::vector<char> &tempLine, RequestInfo &info)
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
		info.multipartHeaders.push_back(multipartHeader);

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
		info.multipartValues.push_back(multipartValue);

		// Ajustar boundaryPos para continuar a busca
		boundaryPos = contentEnd;
	}
}
