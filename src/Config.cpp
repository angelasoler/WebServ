#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Definição e inicialização do ponteiro estático
Config* Config::instance = NULL;

Config* Config::getInstance()
{
	if (instance == NULL) {
		instance = new Config();
	}
	return instance;
}

// Construtores das Structs
RouteConfig::RouteConfig()
    : path(DEFAULT_ROUTE_PATH),
      redirection(DEFAULT_REDIRECTION),
      root_directory(DEFAULT_ROOT_DIRECTORY),
      directory_listing(DEFAULT_DIRECTORY_LISTING),
      default_file(DEFAULT_FILE),
      cgi_extension(DEFAULT_CGI_EXTENSION),
      upload_directory(DEFAULT_UPLOAD_DIRECTORY)
{}

CGIConfig::CGIConfig()
    : path_info(DEFAULT_PATH_INFO),
      script_path(DEFAULT_SCRIPT_PATH)
{}

ServerConfig::ServerConfig()
    : host(DEFAULT_HOST),
      port(DEFAULT_SERVER_PORT),
      default_error_page(DEFAULT_ERROR_PAGE),
      client_body_limit(DEFAULT_CLIENT_BODY_LIMIT),
      cgi()
{}

// Methods
void Config::addServer(const ServerConfig& server)
{
	servers.push_back(server);
}


void Config::configReset()
{
    currentServer = ServerConfig();
    currentRoute = RouteConfig();
    inServer = false;
    inRoute = false;
    usedPorts.clear();
    servers.clear();
}

void Config::loadDefaultConfig(void)
{
	configReset();
	finishServer();
}

void Config::loadConfig(const std::string& configFilePath)
{
	std::ifstream configFile(configFilePath.c_str());
	std::string line;

	configReset();
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
	if (servers.empty())
		finishServer();
	configFile.close();
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
		const char* defaultMethods[] = {DEFAULT_ACCEPTED_METHODS};
		int num_methods = sizeof(defaultMethods) / sizeof(defaultMethods[0]);

		for (int i = 0; i < num_methods; ++i) {
			currentRoute.accepted_methods.push_back(defaultMethods[i]);
		}
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
	if (servers.empty())
	{
		if (currentServer.port < 0)
			currentServer.port = DEFAULT_PORT;
	}
	if (currentServer.port < 0)
	{
		currentServer = ServerConfig();
		return ;
	}
	if (isInPorts(currentServer.port))
	{
		currentServer = ServerConfig();
		return ;
	}
	usedPorts.push_back(currentServer.port);
	if (currentServer.server_names.empty())
		currentServer.server_names.push_back(DEFAULT_SERVER_NAME);
	this->addServer(currentServer);
	currentServer = ServerConfig();
}


