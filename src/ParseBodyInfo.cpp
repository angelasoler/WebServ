#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>


void ParseBodyInfo::parseBodyInfo(RequestInfo &info)
{	
	if (info.action != UPLOAD)
		return;
	if (info.contentType.find("application/x-www-form-urlencoded") != std::string::npos)
		parseBodyValues(info);
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
