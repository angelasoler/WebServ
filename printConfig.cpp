#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Config.hpp"

void printRouteConfig(RouteConfig& routeConfig)
{
	std::cout << "Path: " << routeConfig.path << std::endl;

	std::cout << "Accepted Methods: ";
	for (std::vector<std::string>::iterator it = routeConfig.accepted_methods.begin();
		 it != routeConfig.accepted_methods.end(); ++it)
	{
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

void printCGIConfig(CGIConfig& cgiConfig)
{
	std::cout << "Path Info: " << cgiConfig.path_info << std::endl;
	std::cout << "Script Path: " << cgiConfig.script_path << std::endl;
	std::cout << std::endl;
}

void printServerConfig(ServerConfig& serverConfig)
{
	std::cout << "Host: " << serverConfig.host << std::endl;
	std::cout << "Port: " << serverConfig.port << std::endl;

	std::cout << "Server Names: ";
	for (std::vector<std::string>::iterator it = serverConfig.server_names.begin();
		 it != serverConfig.server_names.end(); ++it) {
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	std::cout << "Default Error Page: " << serverConfig.default_error_page << std::endl;
	std::cout << "Client Body Limit: " << serverConfig.client_body_limit << std::endl;
	printCGIConfig(serverConfig.cgi);

	std::cout << "Routes: " << std::endl;
	for (std::map<std::string, RouteConfig>::iterator it = serverConfig.routes.begin();
		 it != serverConfig.routes.end(); ++it)
	{
		std::cout << "Route Key: " << it->first << std::endl;
		printRouteConfig(it->second);
	}
}

void printConfig(Config& config)
{
	std::cout << "Servers: " << std::endl;
	for (std::vector<ServerConfig>::iterator it = config.servers.begin();
		 it != config.servers.end(); ++it)
	{
		printServerConfig(*it);
	}
}
