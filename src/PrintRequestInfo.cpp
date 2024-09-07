#include "PrintRequestInfo.hpp"
#include "Config.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "TimeNow.hpp"

void PrintRequestInfo::printRequestInfo(RequestInfo& request)
{
	std::ofstream	logFd("logs/requestInfo.log", std::ios_base::app);

	logFd << "\n" << TimeNow();
	logFd << "REQUEST INFO\n";
	logFd << "\tRequested Route: " << request.requestedRoute << std::endl;
	logFd << "\tFull Path: " << request.fullPath << std::endl;
	logFd << "\tPath Type: " << pathTypeToString(request.pathType) << std::endl;
	logFd << "\tAction: " << httpMethodActionToString(request.action) << std::endl;

	logFd << "\tPermissions: " << std::endl;
	logFd << "\t  Read: " << (request.permissions.read ? "true" : "false") << std::endl;
	logFd << "\t  Write: " << (request.permissions.write ? "true" : "false") << std::endl;
	logFd << "\t  Execute: " << (request.permissions.execute ? "true" : "false") << std::endl;
	logFd << "\t  Not Found: " << (request.permissions.notFound ? "true" : "false") << std::endl;

	logFd << "\tBody: " << request.body << std::endl;

	logFd << "\tBody Values:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = request.urlencodedValues.begin(); it != request.urlencodedValues.end(); ++it) {
		logFd << "\t  " << it->first << ": " << it->second << std::endl;
	}

	if (request.multipartHeaders.size() > 0 && request.multipartValues.size() > 0)
	{
		logFd << "\tMultipartBody Headers:" << std::endl;
		for (std::vector<std::string>::iterator it = request.multipartHeaders.begin(); it != request.multipartHeaders.end(); ++it) {
			logFd << "\t  " << *it << std::endl;
		}
		logFd << "\tMultipartBody Values size: " << request.multipartValues.size() << std::endl;
	}
	logFd << "\tserverRef:" << std::endl;

	logFd.close();
	printVectorChar(request.rawBody, "Raw Body", "logs/raw_body.log");
	// printServerConfig(request.serverRef, logFd);
}

void PrintRequestInfo::printVectorChar(std::vector<char> &vec, std::string header, std::string path)
{
	if (vec.empty()) {	
		return ;
	}
	std::ofstream	logFd(path.c_str(), std::ios_base::app);
	bool isDelimiter = false;	
	logFd << "\n" << TimeNow();
	logFd << "--------" + header + " Start-------" << std::endl;
	logFd << "\n";
	for (std::vector<char>::iterator it = vec.begin(); it != vec.end(); ++it)
	{
		if (std::isprint(static_cast<unsigned char>(*it))) {
			logFd << *it;
		} else {
			int c = static_cast<int>(*it);
			logFd << '[' << c << ']';
			if (isDelimiter && c == 10)
				logFd << "\n";
			isDelimiter = false;
			if (c == 13)
				isDelimiter = true;
			
		}
	}
	logFd << "\n--------" + header + " End---------" << std::endl;
	logFd.close();
}

const char* PrintRequestInfo::pathTypeToString(e_pathType pathType) {
	switch (pathType) {
		case File: return "File";
		case Directory: return "Directory";
		case Redirection: return "Redirection";
		case CGI: return "CGI";
		case UNKNOWN: return "UNKNOWN";
		default: return "UNKNOWN";
	}
}

const char* PrintRequestInfo::httpMethodActionToString(e_httpMethodActions action) {
	switch (action) {
		case RESPONSE: return "RESPONSE";
		case UPLOAD: return "UPLOAD";
		case DELETE: return "DELETE";
		case CLOSE: return "CLOSE";
		default: return "UNKNOWN";
	}
}
