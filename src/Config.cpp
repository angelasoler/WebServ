#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

RouteConfig::RouteConfig()
    : path("/"),
      redirection(""),
      root_directory("/var/www/html"),
      directory_listing(false),
      default_file("index.html"),
      cgi_extension(".cgi"),
      upload_directory("/var/www/uploads")
{}


CGIConfig::CGIConfig()
    : path_info("/cgi-bin"),
      script_path("/usr/lib/cgi-bin")
{}

ServerConfig::ServerConfig()
    : host("127.0.0.1"),
      port(-1),
      default_error_page("/error.html"),
      client_body_limit(1048576),
      cgi()
{}

void Config::addServer(const ServerConfig& server)
{
	servers.push_back(server);
}

void Config::loadConfig(const std::string& configFilePath)
{
	std::ifstream configFile(configFilePath.c_str());
	std::string line;
	inRoute = false;
	inServer = false;
	while (std::getline(configFile, line))
	{
		std::istringstream iss(line);
		std::string key;
		std::string value;

		if (line.empty() || line[0] == '#') // skip empty lines and comments
			continue;
		if (!(iss >> key))
			continue;
		if (!inServer)
		{
			if (key == "START_SERVER")
				inServer = 1;
			continue;
		}
		if (key == "END_SERVER")
		{
			inServer = false;
			inRoute = false;
			finishServer();
			continue ;
		}
		if (!inRoute)
		{
			if (key == "START_ROUTE")
			{
				inRoute = true;
				continue ;
			}
		}
		if (key == "END_ROUTE")
		{
			inRoute = false;
			continue ;
		}
		if (iss >> value)
		{
			processServerConfig(key, value);
			processRouteConfig(key, value, iss);
			processCGIConfig(key, value);
		}
	}
	if (inServer)
		finishServer();
}

void Config::processServerConfig(const std::string& key, const std::string& value)
{
	if (key == "server")
		currentServer.host = value;
	else if (key == "port")
		currentServer.port = std::atoi(value.c_str());
	else if (key == "server_name")
		currentServer.server_names.push_back(value);
	else if (key == "error_page")
		currentServer.default_error_page = value;
	else if (key == "client_body_limit")
		currentServer.client_body_limit = std::atoi(value.c_str());
}

void Config::processRouteConfig(const std::string& key, const std::string& value, std::istringstream &iss)
{
	if (key == "route_path")
		currentRoute.path = value;
	else if (key == "accepted_methods")
	{
		std::string method;
		currentRoute.accepted_methods.push_back(value);
		while (iss >> method)
		{
			currentRoute.accepted_methods.push_back(method);
		}
	}
	else if (key == "redirection")
		currentRoute.redirection = value;
	else if (key == "root_directory")
		currentRoute.root_directory = value;
	else if (key == "directory_listing")
		currentRoute.directory_listing = (value == "on");
	else if (key == "default_file")
		currentRoute.default_file = value;
	else if (key == "cgi_extension")
		currentRoute.cgi_extension = value;
	else if (key == "upload_directory")
		currentRoute.upload_directory = value;
}

void Config::processCGIConfig(const std::string& key, const std::string& value)
{
	if (key == "cgi_path_info")
		currentServer.cgi.path_info = value;
	else if (key == "cgi_script_path")
		currentServer.cgi.script_path = value;
}

void Config::finishRoute(void)
{
	if (currentRoute.accepted_methods.empty())
	{
		currentRoute.accepted_methods.push_back("GET");
		currentRoute.accepted_methods.push_back("DELETE");
	}
	currentServer.routes[currentRoute.path] = currentRoute;
	currentRoute = RouteConfig();
}

bool Config::isInPorts(int port)
{
    std::vector<int>::const_iterator it = std::find(usedPorts.begin(), usedPorts.end(), port);
    return it != usedPorts.end();
}

void Config::finishServer(void)
{
	finishRoute();
	if (currentServer.port < 0)
	{
		handleConfigError("Your server need define a PORT");
	}
	if (isInPorts(currentServer.port))
	{
		handleConfigError("Your are using repeted PORT");
	}
	usedPorts.push_back(currentServer.port);
	if (currentServer.server_names.empty())
		currentServer.server_names.push_back("localhost");
	this->addServer(currentServer);
	currentServer = ServerConfig();
}

void	handleConfigError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

