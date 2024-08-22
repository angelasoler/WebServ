#include "PrintRequestInfo.hpp"

void PrintRequestInfo::printRequestInfo(RequestInfo& request) {
	std::cout << "Requested Route: " << request.requestedRoute << std::endl;
	std::cout << "Full Path: " << request.fullPath << std::endl;
	std::cout << "Path Type: " << pathTypeToString(request.pathType) << std::endl;
	std::cout << "Action: " << httpMethodActionToString(request.action) << std::endl;

	std::cout << "Permissions: " << std::endl;
	std::cout << "  Read: " << (request.permissions.read ? "true" : "false") << std::endl;
	std::cout << "  Write: " << (request.permissions.write ? "true" : "false") << std::endl;
	std::cout << "  Execute: " << (request.permissions.execute ? "true" : "false") << std::endl;
	std::cout << "  Not Found: " << (request.permissions.notFound ? "true" : "false") << std::endl;

	std::cout << "Auto Index: " << (request.auto_index ? "true" : "false") << std::endl;

	std::cout << "Boundary: " << request.boundary << std::endl;
	std::cout << "Body: " << request.body << std::endl;
	std::cout << "Content Type: " << request.contentType << std::endl;

	std::cout << "Body Values:" << std::endl;
	for (std::map<std::string, std::string>::iterator it = request.bodyValues.begin(); it != request.bodyValues.end(); ++it) {
		std::cout << "  " << it->first << ": " << it->second << std::endl;
	}

	printServerConfig(request.serverRef);
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

void PrintRequestInfo::printServerConfig(ServerConfig& serverConfig) {
	std::cout << "Host: " << serverConfig.host << std::endl;
	std::cout << "Port: " << serverConfig.port << std::endl;

	std::cout << "Server Names: ";
	std::vector<std::string>::iterator severNameIt;
	for (severNameIt = serverConfig.server_names.begin();
		 severNameIt != serverConfig.server_names.end(); ++severNameIt) {
		std::cout << *severNameIt << " ";
	}
	std::cout << std::endl;

	std::cout << "Default Error Page: " 
		<< serverConfig.default_error_page << std::endl;
	std::cout << "Client Body Limit: " 
		<< serverConfig.client_body_limit << std::endl;

	std::cout << "Routes: " << std::endl;
	std::map<std::string, RouteConfig>::iterator routeIt;
	for (routeIt = serverConfig.routes.begin();
		 routeIt != serverConfig.routes.end(); ++routeIt) {
		std::cout << "Route Key: " << routeIt->first << std::endl;
		printRouteConfig(routeIt->second);
	}
}

void PrintRequestInfo::printRouteConfig(RouteConfig& routeConfig) {
	std::cout << "Path: " << routeConfig.route << std::endl;

	std::cout << "Accepted Methods: ";
	for (std::vector<std::string>::iterator it = routeConfig.accepted_methods.begin();
		 it != routeConfig.accepted_methods.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	std::cout << "Redirection: " << routeConfig.redirection << std::endl;
	std::cout << "Root Directory: " << routeConfig.root_directory << std::endl;
	std::cout << "Directory Listing: " << (routeConfig.directory_listing ? "Yes" : "No") << std::endl;
	std::cout << "Default File: " << routeConfig.default_file << std::endl;
	std::cout << "CGI Extension: " << routeConfig.cgi_extension << std::endl;
	std::cout << "Upload Directory: " << routeConfig.upload_directory << std::endl;
	std::cout << std::endl;
}
