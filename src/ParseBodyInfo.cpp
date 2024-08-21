#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

void ParseBodyInfo::parseBodyInfo(std::string requestText, RequestInfo &info)
{	
	if (info.action != UPLOAD)
		return;
	extractBody(requestText, info);
	if (info.contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		parseBodyValues(info);
}

void extractBody(std::string requestText, RequestInfo &info)
{
	// Substitue todo \r\n por \n
	std::string newDivisor = "\n";
	info.body = stringWithNewDivisor(requestText, newDivisor);

	// \r\n\r\n se é representada por \n\n
	std::string finalDivisor = newDivisor + newDivisor;


	// tenta encontrar o equivalente a \r\n\r\n no texto, ou seja tenta encontrar \n\n

	// info.body = requestText;
	// std::string finalDivisor = "\r\n\r\n";
	std::size_t pos = info.body.find(finalDivisor);
	if (pos != std::string::npos) {
		// Verifica se há algo após "\r\n\r\n"
		if (pos + finalDivisor.size() < info.body.size()) {
			std::string posDivisor = info.body.substr(pos + finalDivisor.size());
			info.body = posDivisor;

			// std::cout << info.body << "\n";
		}
		else {
			// "Não há nada após a sequência \\r\\n\\r\\n.
		}
	}
	else {
		// "A sequência \\r\\n\\r\\n não foi encontrada no texto
	}
}

void parseBodyValues(RequestInfo &info)
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
				info.bodyValues[key] = value;
			}
			else {
				std::cerr << "Error: Encountered an empty key in the request body." << std::endl;
			}
		}
		else {
			std::cerr << "Error: Malformed key-value pair '" << pair << "' in the request body." << std::endl;
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
			info.bodyValues[key] = value;
		}
		else {
			std::cerr << "Error: Encountered an empty key in the request body." << std::endl;
		}
	}
	else if (!pair.empty()) {
		std::cerr << "Error: Malformed key-value pair '" << pair << "' in the request body." << std::endl;
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

std::string stringWithNewDivisor(std::string text, std::string newDivisor)
{
	std::string result;

	size_t pos = 0;
	size_t oldDivisorPos;
	
	while ((oldDivisorPos = text.find("\r\0", pos)) != std::string::npos)
	{
		result.append(text, pos, oldDivisorPos - pos);
		result.append(newDivisor);
		pos = oldDivisorPos + 2;
	}
	result.append(text, pos);
	return result;
}
