#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

static void parseRequestBodyMultipart(RequestInfo &info);
static void parseMultipartInfo(const std::string& boundary, std::string &body, RequestInfo &info);

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
		parseMultipartInfo(boundary, info.body, info);
	}
}

static void parseMultipartInfo(const std::string& boundary, std::string &body, RequestInfo &info)
{
	size_t boundaryPos = 0;
	size_t contentStart = 0;
	std::string multipartHeader;

	while ((boundaryPos = body.find(boundary, boundaryPos)) != std::string::npos)
	{
		contentStart = body.find("\r\n\r\n", boundaryPos);
		if (contentStart == std::string::npos)
		{
			return;
		}

		multipartHeader = body.substr(boundaryPos + boundary.size(), contentStart - (boundaryPos + boundary.size()));
		// std::cerr << "Multipart header:\n\t" << multipartHeader << std::endl;
		info.multipartHeaders.push_back(multipartHeader);

		contentStart += 4; // Pular "\r\n\r\n"
		if (contentStart >= body.size())
		{
			return;
		}

		size_t contentEnd = body.find(boundary, contentStart);
		if (contentEnd == std::string::npos)
		{
			contentEnd = body.size();
		}

		// Remover possíveis '\r', '\n' e '-' do final do conteúdo
		while (contentEnd > contentStart && (body[contentEnd - 1] == '\r' || body[contentEnd - 1] == '\n' || body[contentEnd - 1] == '-'))
		{
			--contentEnd;
		}

		std::string multipartValue = body.substr(contentStart, contentEnd - contentStart);
		// std::cerr << "Multipart body:\n\t" << multipartValues << std::endl;
		info.multipartValues.push_back(multipartValue);

		// Atualizar body para o conteúdo restante após o bodypart
		body = body.substr(contentEnd);

		// Ajustar boundaryPos para a próxima ocorrência do boundary
		boundaryPos = 0;
	}
}
