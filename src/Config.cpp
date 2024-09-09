#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

// Static pointer initialization
Config* Config::instance = NULL;

Config* Config::getInstance()
{
	if (instance == NULL) {
		instance = new Config();
	}
	return instance;
}

// Static pointer endalization
void	Config::freeInstance()
{
	if (instance != NULL)
	{
		delete instance;
		instance = NULL;
	}
}

// Struct Constructors
RouteConfig::RouteConfig()
	: route(DEFAULT_ROUTE_PATH),
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
	  default_error_page(),
	  client_body_limit(DEFAULT_CLIENT_BODY_LIMIT),
	  cgi()
{}

// Public Methods
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

void Config::loadDefaultConfig()
{
	configReset();
	finishRoute();
	finishServer();
}

void Config::loadConfig(const std::string& configFilePath)
{
	std::ifstream configFile(configFilePath.c_str());
	std::string line;

	configReset();

	while (std::getline(configFile, line))
	{
		processConfigLine(line);
	}

	finalizeConfigParsing();
	configFile.close();
}

// Private Methods
void Config::processConfigLine(const std::string& line)
{
	std::istringstream iss(line);
	std::string key;
	std::string value;

	if (line.empty() || line[0] == '#') // Skip empty lines and comments
		return;
	if (!(iss >> key))
		return;

	if (key == "START_SERVER")
		handleStartServer();
	else if (key == "END_SERVER")
		handleEndServer();
	else if (key == "START_ROUTE")
		handleStartRoute();
	else if (key == "END_ROUTE")
		handleEndRoute();
	else if (inServer && (iss >> value))
	{
		processServerConfig(key, value, iss);
		processRouteConfig(key, value, iss);
		processCGIConfig(key, value);
	}
}

void Config::handleStartServer()
{
	if (!inServer) {
		inServer = true;
	}
}

void Config::handleEndServer()
{
	if (inServer) {
		if (inRoute) {
			finishRoute();
			inRoute = false; // Finalize any open route before ending the server
		}
		finishServer();
		inServer = false;
	}
}

void Config::handleStartRoute()
{
	if (!inRoute) {
		inRoute = true;
	}
}

void Config::handleEndRoute()
{
	if (inRoute) {
		finishRoute();
		inRoute = false;
	}
}

void Config::finalizeConfigParsing()
{
	if (inServer) {
		if (inRoute)
			finishRoute();
		finishServer();
	} else if (servers.empty()) {
		finishServer();
	}
}

void Config::processServerConfig(const std::string& key, const std::string& value, std::istringstream &iss)
{
	if (key == "server")
		currentServer.host = value;
	else if (key == "port")
		currentServer.port = std::atoi(value.c_str());
	else if (key == "server_name")
		currentServer.server_names.push_back(value);
	else if (key == "error_page") {
		int	code;
		std::string default_page;
		std::istringstream(value) >> code;
		iss >> default_page;
		currentServer.default_error_page[code] = default_page;
	}
	else if (key == "client_body_limit")
		currentServer.client_body_limit = std::atoi(value.c_str());
}

void Config::processRouteConfig(const std::string& key, const std::string& value, std::istringstream &iss)
{
	if (!inRoute)
		return ;
	if (key == "route_path")
		currentRoute.route = value;
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

void Config::finishRoute()
{
	if (currentRoute.accepted_methods.empty())
	{
		const char* defaultMethods[] = {DEFAULT_ACCEPTED_METHODS};
		int num_methods = sizeof(defaultMethods) / sizeof(defaultMethods[0]);

		for (int i = 0; i < num_methods; ++i) {
			currentRoute.accepted_methods.push_back(defaultMethods[i]);
		}
	}
	currentServer.routes[currentRoute.route] = currentRoute;
	currentRoute = RouteConfig();
}

void Config::finishServer()
{
	if (currentServer.port < 0) {
		currentServer.port = DEFAULT_PORT;
	}
	if (isReservedPort(currentServer.port)) {
		currentServer = ServerConfig();
		return ;
	}
	usedPorts.push_back(currentServer.port);
	if (currentServer.server_names.empty())
		currentServer.server_names.push_back(DEFAULT_SERVER_NAME);
	if (currentServer.routes.empty())
		finishRoute();
	this->addServer(currentServer);
	currentServer = ServerConfig();
	inRoute = false;
	inServer = false;
}

bool Config::isReservedPort(int port)
{
	if (port < 1024)
		return true;
	if (std::find(usedPorts.begin(), usedPorts.end(), port) != usedPorts.end())
		return true;
	return false;
}
