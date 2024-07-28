#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

void Config::addServer(const ServerConfig& server)
{
	servers.push_back(server);
}

void Config::processServerConfig(const std::string& key, const std::string& value)
{
	if (key == "server")
	{
		if (!currentServer.host.empty()) // caso já o campo 'host' nao esteja vazio, as variaveis CurrentServer, CurrentRoute e CurrentCGI sao armazenadas e em seguida resetadas.
		{
			if (!currentRoute.path.empty())
			{
				currentServer.routes[currentRoute.path] = currentRoute;
				currentRoute = RouteConfig();
			}
			if (!currentCGI.path_info.empty())
			{
				currentServer.cgi = currentCGI;
				currentCGI = CGIConfig();
			}
			this->addServer(currentServer);
			currentServer = ServerConfig();
		}
		currentServer.host = value;
	}
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
	{
		if (!currentRoute.path.empty())
		{
			currentServer.routes[currentRoute.path] = currentRoute;
			currentRoute = RouteConfig();
		}
		currentRoute.path = value;
	}
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
	{
		currentRoute.directory_listing = (value == "on");
	}
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
	{
		if (!currentCGI.path_info.empty())
		{
			currentServer.cgi = currentCGI;
			currentCGI = CGIConfig();
		}
		currentCGI.path_info = value;
	}
	else if (key == "cgi_script_path")
	{
		currentCGI.script_path = value;
	}
}

void Config::loadConfig(const std::string& configFilePath)
{
	std::ifstream configFile(configFilePath.c_str());
	std::string line;

	while (std::getline(configFile, line))
	{
		std::istringstream iss(line);
		std::string key;
		std::string value;

		if (line.empty() || line[0] == '#') // skip empty lines and comments
			continue;

		if (iss >> key >> value)
		{
			processServerConfig(key, value);
			processRouteConfig(key, value, iss);
			processCGIConfig(key, value);
		}
	}

	if (!currentRoute.path.empty())
	{
		currentServer.routes[currentRoute.path] = currentRoute;
	}

	if (!currentCGI.path_info.empty())
	{
		currentServer.cgi = currentCGI;
	}

	if (!currentServer.host.empty())
	{
		this->addServer(currentServer);
	}
}
