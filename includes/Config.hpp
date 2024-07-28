#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
struct RouteConfig
{
	std::string					path;
	std::vector<std::string>	accepted_methods;
	std::string					redirection;
	std::string					root_directory;
	bool 						directory_listing;
	std::string 				default_file;
	std::string 				cgi_extension;
	std::string					upload_directory;

	RouteConfig();
};

struct CGIConfig
{
	std::string path_info;
	std::string script_path;

	CGIConfig();
};

struct ServerConfig
{
	std::string					host;
	int 						port;
	std::vector<std::string>	server_names;
	std::string					default_error_page;
	size_t						client_body_limit;
	CGIConfig					cgi;
	std::map<std::string, RouteConfig>	routes;

	ServerConfig();
};

class Config
{
	private:
		ServerConfig	currentServer;
		RouteConfig		currentRoute;
		bool			inServer;
		bool			inRoute;
	public:
		std::vector<ServerConfig>	servers;
		void	finish_server(void);
		void	finish_route(void);
		void	processServerConfig(const std::string& key, const std::string& value);
		void	processRouteConfig(const std::string& key, const std::string& value, std::istringstream &iss);
		void	processCGIConfig(const std::string& key, const std::string& value);
		void	loadConfig(const std::string& configFilePath);
		void	addServer(const ServerConfig& server);
};

void printConfig(Config& config);

#endif //CONFIG_HPP
