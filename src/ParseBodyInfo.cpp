#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

void parseRequest(const std::string& requestText, RequestInfo& requestInfo);

void ParseBodyInfo::parseBodyInfo(std::string requestText, RequestInfo &info)
{	
	(void)requestText;
	(void)info;
	parseRequest(requestText, info);
}

void parseRequest(const std::string& requestText, RequestInfo& requestInfo) {
	std::istringstream requestStream(requestText);
	std::string line;
	
	// Parse header lines
	while (std::getline(requestStream, line) && !line.empty()) {
		// Remove '\r' at the end of each line if it exists
		if (!line.empty() && line[line.size() - 1] == '\r') {
			line.erase(line.size() - 1);
		}

		// Check for Content-Type header
		if (line.find("Content-Type:") != std::string::npos) {
			requestInfo.contentType = line.substr(14); // 14 is the length of "Content-Type: "
		}
	}
	
	// Parse the body if it exists
	if (std::getline(requestStream, line)) {
		requestInfo.body = line;
		
		// If Content-Type is application/x-www-form-urlencoded, parse the body into the map
		if (requestInfo.contentType == "application/x-www-form-urlencoded") {
			std::istringstream bodyStream(requestInfo.body);
			std::string keyValuePair;
			
			while (std::getline(bodyStream, keyValuePair, '&')) {
				size_t pos = keyValuePair.find('=');
				if (pos != std::string::npos) {
					std::string key = keyValuePair.substr(0, pos);
					std::string value = keyValuePair.substr(pos + 1);
					requestInfo.requestBody[key] = value;
				}
			}
		}
	}
}
