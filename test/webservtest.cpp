#include <gtest/gtest.h>
#include <cstdlib>
#include <thread>
#include <fstream>

void start_server() {
	std::system("./webserv > server-outputs.log &");
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void start_server_with_conf(std::string configFile) {
	std::string fullPath = std::string("./webserv ") + std::string(configFile) + std::string(" > server-outputs.log &");
	std::system(fullPath.c_str());
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void stop_server() {
	std::system("pkill --signal SIGINT webserv");
}

std::string exec(std::string command) {
	char		buffer[128];
	std::string	result = "";

	FILE* pipe = popen(command.c_str(), "r");
	if (!pipe) {
		return "popen failed!";
	}

	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}

	pclose(pipe);
	return result;
}

// TEST(HttpTest, get500) {

// //multiplas conexoe
// TEST(HttpTest, multiConecction) {

// TEST(HttpTest, CurlGetStatusCodes) {
// 	//ARRANGE
// 	- Startar Server
// 	- fazer uma requisição GET / com curl //valida
// 	- fazer uma requisição GET /caminho que não existe
// 	- parsea stdout do crul
// 	//ASSERT
// 	-verificar codigo 200
// }

TEST(HttpTest, FirefoxGetRequest) {
	// ARRANGE
	start_server();
	// ACT
	int result = std::system("test/test-firefox");

	// ASSERT
	EXPECT_EQ(result, 0);
	stop_server();
}

TEST(HttpTest, TelnetGetRequest) {
	start_server();
	std::string response = exec("test/test-telnet");

	EXPECT_NE(response.find("Hello"), std::string::npos);
	stop_server();
}

#include "Config.hpp"

TEST(ConfigTest, NoServersFile) {
	Config  *config = Config::getInstance();

	config->loadDefaultConfig();
	ASSERT_EQ(config->servers.size(), 1);
	
	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, DEFAULT_HOST);
	EXPECT_EQ(server1.port, DEFAULT_PORT);
	EXPECT_EQ(server1.server_names[0], DEFAULT_SERVER_NAME);
	EXPECT_EQ(server1.default_error_page, DEFAULT_ERROR_PAGE);
	EXPECT_EQ(server1.client_body_limit, DEFAULT_CLIENT_BODY_LIMIT);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	EXPECT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, false);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);
	EXPECT_EQ(route1.cgi_extension, DEFAULT_CGI_EXTENSION);
	EXPECT_EQ(route1.upload_directory, DEFAULT_UPLOAD_DIRECTORY);

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, DEFAULT_PATH_INFO);
	EXPECT_EQ(server1.cgi.script_path, DEFAULT_SCRIPT_PATH);
}

TEST(ConfigTest, NoServers) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/NoServers.conf");
	ASSERT_EQ(config->servers.size(), 1);
	
	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, DEFAULT_HOST);
	EXPECT_EQ(server1.port, DEFAULT_PORT);
	EXPECT_EQ(server1.server_names[0], DEFAULT_SERVER_NAME);
	EXPECT_EQ(server1.default_error_page, DEFAULT_ERROR_PAGE);
	EXPECT_EQ(server1.client_body_limit, DEFAULT_CLIENT_BODY_LIMIT);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	EXPECT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, false);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);
	EXPECT_EQ(route1.cgi_extension, DEFAULT_CGI_EXTENSION);
	EXPECT_EQ(route1.upload_directory, DEFAULT_UPLOAD_DIRECTORY);

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, DEFAULT_PATH_INFO);
	EXPECT_EQ(server1.cgi.script_path, DEFAULT_SCRIPT_PATH);
}

TEST(ConfigTest, OneServer) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/OneServer.conf");

	// printConfig(*config);
	// return ;
	ASSERT_EQ(config->servers.size(), 1);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");
}

TEST(ConfigTest, TwoServers) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/TwoServers.conf");
	ASSERT_EQ(config->servers.size(), 2);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");

	// Verificação do segundo servidor
	const ServerConfig& server2 = config->servers[1];
	EXPECT_EQ(server2.host, "192.168.1.1");
	EXPECT_EQ(server2.port, 9090);
	EXPECT_EQ(server2.server_names[0], "example_y.com");
	EXPECT_EQ(server2.default_error_page, "/errors/500.html");
	EXPECT_EQ(server2.client_body_limit, 2097152);

	// Verificação da rota do segundo servidor
	ASSERT_EQ(server2.routes.size(), 1);
	const RouteConfig& route2 = server2.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");
	EXPECT_EQ(route2.accepted_methods.size(), 1);
	EXPECT_EQ(route2.accepted_methods[0], "GET");
	EXPECT_EQ(route2.directory_listing, false);
	EXPECT_EQ(route2.default_file, "index.html");
	EXPECT_EQ(route2.cgi_extension, ".php");
	EXPECT_EQ(route2.upload_directory, "/var/www/route_y_uploads");

	// Verificação do CGI do segundo servidor
	EXPECT_EQ(server2.cgi.path_info, "/cgi-bin/route_y");
	EXPECT_EQ(server2.cgi.script_path, "/usr/lib/cgi-bin/route_y.cgi");
}

TEST(ConfigTest, ThreeServers) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/ThreeServers.conf");
	ASSERT_EQ(config->servers.size(), 3);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");

	const ServerConfig& server2 = config->servers[1];
	EXPECT_EQ(server2.host, "192.168.1.1");
	EXPECT_EQ(server2.port, 9999);
	EXPECT_EQ(server2.server_names[0], "example_y.com");
	EXPECT_EQ(server2.default_error_page, "/errors/500.html");
	EXPECT_EQ(server2.client_body_limit, 2097152);

	// Verificação da rota do segundo servidor
	ASSERT_EQ(server2.routes.size(), 1);
	const RouteConfig& route2 = server2.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");
	EXPECT_EQ(route2.accepted_methods.size(), 1);
	EXPECT_EQ(route2.accepted_methods[0], "GET");
	EXPECT_EQ(route2.directory_listing, false);
	EXPECT_EQ(route2.default_file, "index.html");
	EXPECT_EQ(route2.cgi_extension, ".php");
	EXPECT_EQ(route2.upload_directory, "/var/www/route_y_uploads");

	EXPECT_EQ(server2.cgi.path_info, "/cgi-bin/route_y");
	EXPECT_EQ(server2.cgi.script_path, "/usr/lib/cgi-bin/route_y.cgi");

	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");

	// Verificação do terceiro servidor
	const ServerConfig& server3 = config->servers[2];
	EXPECT_EQ(server3.host, "192.168.1.1");
	EXPECT_EQ(server3.port, 9090);
	EXPECT_EQ(server3.server_names[0], "example_z.com");
	EXPECT_EQ(server3.default_error_page, "/errors/500.html");
	EXPECT_EQ(server3.client_body_limit, 2097152);

	ASSERT_EQ(server3.routes.size(), 1);
	const RouteConfig& route3 = server3.routes.at("/route_z");
	EXPECT_EQ(route3.route, "/route_z");
	EXPECT_EQ(route3.root_directory, "/var/www/route_z");
	EXPECT_EQ(route3.accepted_methods.size(), 1);
	EXPECT_EQ(route3.accepted_methods[0], "GET");
	EXPECT_EQ(route3.directory_listing, false);
	EXPECT_EQ(route3.default_file, "index.html");
	EXPECT_EQ(route3.cgi_extension, ".php");
	EXPECT_EQ(route3.upload_directory, "/var/www/route_z_uploads");

	EXPECT_EQ(server3.cgi.path_info, "/cgi-bin/route_z");
	EXPECT_EQ(server3.cgi.script_path, "/usr/lib/cgi-bin/route_z.cgi");
}

TEST(ConfigTest, NoHostOnDefaultServer) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/NoHostOnDefaultServer.conf");
	ASSERT_EQ(config->servers.size(), 2);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");

	// Verificação do segundo servidor
	const ServerConfig& server2 = config->servers[1];
	EXPECT_EQ(server2.host, "192.168.1.1");
	EXPECT_EQ(server2.port, 9090);
	EXPECT_EQ(server2.server_names[0], "example_y.com");
	EXPECT_EQ(server2.default_error_page, "/errors/500.html");
	EXPECT_EQ(server2.client_body_limit, 2097152);

	// Verificação da rota do segundo servidor
	ASSERT_EQ(server2.routes.size(), 1);
	const RouteConfig& route2 = server2.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");
	EXPECT_EQ(route2.accepted_methods.size(), 1);
	EXPECT_EQ(route2.accepted_methods[0], "GET");
	EXPECT_EQ(route2.directory_listing, false);
	EXPECT_EQ(route2.default_file, "index.html");
	EXPECT_EQ(route2.cgi_extension, ".php");
	EXPECT_EQ(route2.upload_directory, "/var/www/route_y_uploads");

	// Verificação do CGI do segundo servidor
	EXPECT_EQ(server2.cgi.path_info, "/cgi-bin/route_y");
	EXPECT_EQ(server2.cgi.script_path, "/usr/lib/cgi-bin/route_y.cgi");
}

TEST(ConfigTest, InvalidServerId1) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/InvalidServerId1.conf");
	ASSERT_EQ(config->servers.size(), 1);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	// Verificação da rota do primeiro servidor
	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	// Verificação do CGI do primeiro servidor
	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");
}

TEST(ConfigTest, InvalidServerId1ValidServerId2) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/InvalidServerId1ValidServerId2.conf");
	ASSERT_EQ(config->servers.size(), 2);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, "localhost");
	EXPECT_EQ(server1.port, 8080);
	EXPECT_EQ(server1.server_names[0], "example_x.com");
	EXPECT_EQ(server1.default_error_page, "/errors/404.html");
	EXPECT_EQ(server1.client_body_limit, 1048576);

	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");
	EXPECT_EQ(route1.accepted_methods.size(), 3);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "POST");
	EXPECT_EQ(route1.accepted_methods[2], "DELETE");
	EXPECT_EQ(route1.directory_listing, true);
	EXPECT_EQ(route1.default_file, "index.html");
	EXPECT_EQ(route1.cgi_extension, ".php");
	EXPECT_EQ(route1.upload_directory, "/var/www/uploads");

	EXPECT_EQ(server1.cgi.path_info, "/cgi/script");
	EXPECT_EQ(server1.cgi.script_path, "/cgi/script.cgi");

	// Verificação do terceiro servidor
	const ServerConfig& server3 = config->servers[1];
	EXPECT_EQ(server3.host, "192.168.1.1");
	EXPECT_EQ(server3.port, 9090);
	EXPECT_EQ(server3.server_names[0], "example_z.com");
	EXPECT_EQ(server3.default_error_page, "/errors/500.html");
	EXPECT_EQ(server3.client_body_limit, 2097152);

	ASSERT_EQ(server3.routes.size(), 1);
	const RouteConfig& route3 = server3.routes.at("/route_z");
	EXPECT_EQ(route3.route, "/route_z");
	EXPECT_EQ(route3.root_directory, "/var/www/route_z");
	EXPECT_EQ(route3.accepted_methods.size(), 1);
	EXPECT_EQ(route3.accepted_methods[0], "GET");
	EXPECT_EQ(route3.directory_listing, false);
	EXPECT_EQ(route3.default_file, "index.html");
	EXPECT_EQ(route3.cgi_extension, ".php");
	EXPECT_EQ(route3.upload_directory, "/var/www/route_z_uploads");

	EXPECT_EQ(server3.cgi.path_info, "/cgi-bin/route_z");
	EXPECT_EQ(server3.cgi.script_path, "/usr/lib/cgi-bin/route_z.cgi");
}

#include <gtest/gtest.h>
#include "ParsePathInfo.hpp" // Supondo que RequestInfo esteja declarada aqui

// Função que será testada (deve ser implementada por você)
RequestInfo parseHttpRequest(const std::string& httpRequest)
{
	Request			request;
	Config  		*config = Config::getInstance();

	config->loadDefaultConfig();
	ServerConfig server = config->servers[0];

	request.requestsText = httpRequest;
	request.parseRequest(server);
	return request.info;
}

TEST(RequestInfoTest, HandlesGetRequest) {
    std::string getRequest = 
        "GET /index.html HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n\r\n";

    RequestInfo requestInfo = parseHttpRequest(getRequest);

    // Verificando o preenchimento da struct
    EXPECT_EQ(requestInfo.requestedRoute, "/index.html");
    EXPECT_EQ(requestInfo.action, RESPONSE); // Supondo que GET mapeie para RESPONSE
    EXPECT_EQ(requestInfo.pathType, File); // Supondo que "/index.html" seja reconhecido como File
    EXPECT_EQ(requestInfo.permissions.read, true);
    EXPECT_EQ(requestInfo.permissions.write, false);
    EXPECT_EQ(requestInfo.permissions.execute, false);
    EXPECT_EQ(requestInfo.auto_index, false);
    EXPECT_EQ(requestInfo.body, "");
}

TEST(RequestInfoTest, HandlesPostRequest) {
    std::string postRequest = 
        "POST /submit HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/x-www-form-urlencoded\r\n"
        "Content-Length: 27\r\n\r\n"
        "name=John&age=30&city=NYC";

    RequestInfo requestInfo = parseHttpRequest(postRequest);

    // Verificando o preenchimento da struct
    EXPECT_EQ(requestInfo.requestedRoute, "/submit");
    EXPECT_EQ(requestInfo.action, UPLOAD); // Supondo que POST mapeie para UPLOAD
    EXPECT_EQ(requestInfo.pathType, URL); // Supondo que "/submit" seja reconhecido como URL
    EXPECT_EQ(requestInfo.permissions.read, true);
    EXPECT_EQ(requestInfo.permissions.write, true);
    EXPECT_EQ(requestInfo.permissions.execute, false);
    EXPECT_EQ(requestInfo.auto_index, false);
    EXPECT_EQ(requestInfo.body, "name=John&age=30&city=NYC");
    // EXPECT_EQ(requestInfo.requestBody["name"][0], "John");
    // EXPECT_EQ(requestInfo.requestBody["age"][0], "30");
    // EXPECT_EQ(requestInfo.requestBody["city"][0], "NYC");
}

TEST(RequestInfoTest, HandlesDeleteRequest) {
    std::string deleteRequest = 
        "DELETE /remove HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Connection: close\r\n\r\n";

    RequestInfo requestInfo = parseHttpRequest(deleteRequest);

    // Verificando o preenchimento da struct
    EXPECT_EQ(requestInfo.requestedRoute, "/remove");
    EXPECT_EQ(requestInfo.action, DELETE);
    EXPECT_EQ(requestInfo.pathType, File); // Supondo que "/remove" seja reconhecido como File
    EXPECT_EQ(requestInfo.permissions.read, false);
    EXPECT_EQ(requestInfo.permissions.write, true);
    EXPECT_EQ(requestInfo.permissions.execute, false);
    EXPECT_EQ(requestInfo.auto_index, false);
    EXPECT_EQ(requestInfo.body, "");
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
