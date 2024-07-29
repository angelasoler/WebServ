#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#define DEFAULT_PORT 8080

// Definições de macros para RouteConfig
#define DEFAULT_ROUTE_PATH "/"
#define DEFAULT_REDIRECTION ""
#define DEFAULT_ROOT_DIRECTORY "/var/www/html"
#define DEFAULT_DIRECTORY_LISTING false
#define DEFAULT_FILE "index.html"
#define DEFAULT_CGI_EXTENSION ".cgi"
#define DEFAULT_UPLOAD_DIRECTORY "/var/www/uploads"
#define DEFAULT_ACCEPTED_METHODS "GET", "DELETE"

// Definições de macros para CGIConfig
#define DEFAULT_PATH_INFO "/cgi-bin"
#define DEFAULT_SCRIPT_PATH "/usr/lib/cgi-bin"

// Definições de macros para ServerConfig
#define DEFAULT_HOST "127.0.0.1"
#define DEFAULT_SERVER_PORT -1
#define DEFAULT_ERROR_PAGE "/error.html"
#define DEFAULT_CLIENT_BODY_LIMIT 1048576
#define DEFAULT_SERVER_NAME "localhost"

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
		ServerConfig		currentServer;
		RouteConfig			currentRoute;
		bool				inServer;
		bool				inRoute;
		std::vector<int>	usedPorts;
		bool 				isInPorts(int port);
		void	finishServer(void);
		void	finishRoute(void);
		void	processServerConfig(const std::string& key, const std::string& value);
		void	processRouteConfig(const std::string& key, const std::string& value, std::istringstream &iss);
		void	processCGIConfig(const std::string& key, const std::string& value);
		void	addServer(const ServerConfig& server);
		void	configReset();

		// Construtor privado para impedir a criação de instâncias fora da classe
		Config() {}
		// Construtor de cópia e operador de atribuição privados para impedir a cópia
		Config(const Config&);
		Config& operator=(const Config&);

    public:
        std::vector<ServerConfig> servers;

        // Ponteiro estático para a instância
        static Config* instance;

        // Método estático para acessar a única instância
        static Config* getInstance();

        void    loadConfig(const std::string& configFilePath);
};

void	printConfig(Config& config);

#endif //CONFIG_HPP
