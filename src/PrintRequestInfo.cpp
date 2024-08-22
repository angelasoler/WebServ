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

	logFd << "\tAuto Index: " << (request.auto_index ? "true" : "false") << std::endl;

	logFd << "\tBoundary: " << request.boundary << std::endl;
	logFd << "\tBody: " << request.body << std::endl;
	logFd << "\tContent Type: " << request.contentType << std::endl;

	logFd << "\tBody Values:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = request.bodyValues.begin(); it != request.bodyValues.end(); ++it) {
		logFd << "\t  " << it->first << ": " << it->second << std::endl;
	}

	logFd << "\tserverRef:" << std::endl;

	// printServerConfig(request.serverRef, logFd);
}

const char* PrintRequestInfo::pathTypeToString(e_pathType pathType) {
	switch (pathType) {
		case File: return "File";
		case Directory: return "Directory";
		case URL: return "URL";
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
