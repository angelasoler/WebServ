#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "Config.hpp"
#include "TimeNow.hpp"

void printRouteConfig(RouteConfig& routeConfig, std::ofstream &fd)
{
	fd << "Path: " << routeConfig.route << std::endl;

	fd << "Accepted Methods: ";
	for (std::vector<std::string>::iterator it = routeConfig.accepted_methods.begin();
		 it != routeConfig.accepted_methods.end(); ++it)
	{
		fd << *it << " ";
	}
	fd << std::endl;

	fd << "Redirection: " << routeConfig.redirection << std::endl;
	fd << "Root Directory: " << routeConfig.root_directory << std::endl;
	fd << "Directory Listing: " << (routeConfig.directory_listing ? "Yes" : "No") << std::endl;
	fd << "Default File: " << routeConfig.default_file << std::endl;
	fd << "CGI Extension: " << routeConfig.cgi_extension << std::endl;
	fd << "Upload Directory: " << routeConfig.upload_directory << std::endl;
	fd << std::endl;
}

void printServerConfig(ServerConfig& serverConfig, std::ofstream &fd)
{
	fd << "Host: " << serverConfig.host << std::endl;
	fd << "Port: " << serverConfig.port << std::endl;

	fd << "Server Names: ";
	std::vector<std::string>::iterator severNameIt;
	for (severNameIt = serverConfig.server_names.begin();
		 severNameIt != serverConfig.server_names.end(); ++severNameIt) {
		fd << *severNameIt << " ";
	}
	fd << std::endl;

	fd << "Default Error Page: " 
		<< serverConfig.default_error_page << std::endl;
	fd << "Client Body Limit: " 
		<< serverConfig.client_body_limit << std::endl;

	fd << "Routes: " << std::endl;
	std::map<std::string, RouteConfig>::iterator routeIt;
	for (routeIt = serverConfig.routes.begin();
		 routeIt != serverConfig.routes.end(); ++routeIt)
	{
		fd << "Route Key: " << routeIt->first << std::endl;
		printRouteConfig(routeIt->second, fd);
	}
}

void printConfig(void)
{
	Config	*config = Config::getInstance();
	std::ofstream	configLog("logs/config.log", std::ios_base::app);

	configLog << TimeNow() << std::endl;
	configLog << "Servers: " << std::endl;
	int i = 0;
	for (std::vector<ServerConfig>::iterator it = config->servers.begin();
		 it != config->servers.end(); ++it)
	{
		configLog << "\t==== Server "
				<< ++i
				<< "====="
				<< std::endl;
		printServerConfig(*it, configLog);
	}
	configLog.close();
}
