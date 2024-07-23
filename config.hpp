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
};

struct CGIConfig
{
	std::string path_info;
	std::string script_path;
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
};

class config
{
	public:
		std::vector<ServerConfig> servers;

		void	loadConfig(const std::string& configFilePath);
		void	addServer(const ServerConfig& server);
};
