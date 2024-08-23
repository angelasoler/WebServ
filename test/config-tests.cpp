
#include "tests.hpp"
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
	ASSERT_EQ(server1.routes.empty(), false);

	// return ;
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	ASSERT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, DEFAULT_DIRECTORY_LISTING);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);
	EXPECT_EQ(route1.upload_directory, DEFAULT_UPLOAD_DIRECTORY);
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
	ASSERT_EQ(server1.routes.empty(), false);

	
	// return ;
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	ASSERT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, DEFAULT_DIRECTORY_LISTING);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);
	EXPECT_EQ(route1.upload_directory, DEFAULT_UPLOAD_DIRECTORY);
}


TEST(ConfigTest, OneServer) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/OneServer.conf");

	// printConfig();
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

TEST(ConfigTest, InvalidServerId1)
{
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/InvalidServerId1.conf");
	ASSERT_EQ(config->servers.size(), 1);

	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, DEFAULT_HOST);
	EXPECT_EQ(server1.port, DEFAULT_PORT);
	EXPECT_EQ(server1.server_names[0], DEFAULT_SERVER_NAME);
	EXPECT_EQ(server1.default_error_page, DEFAULT_ERROR_PAGE);
	EXPECT_EQ(server1.client_body_limit, DEFAULT_CLIENT_BODY_LIMIT);

	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	EXPECT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, DEFAULT_DIRECTORY_LISTING);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);
}

TEST(ConfigTest, InvalidServerId1ValidServerId2) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/InvalidServerId1ValidServerId2.conf");
	ASSERT_EQ(config->servers.size(), 2);

	// Verificação do primeiro servidor
	const ServerConfig& server1 = config->servers[0];
	EXPECT_EQ(server1.host, DEFAULT_HOST);
	EXPECT_EQ(server1.port, DEFAULT_PORT);
	EXPECT_EQ(server1.server_names[0], DEFAULT_SERVER_NAME);
	EXPECT_EQ(server1.default_error_page, DEFAULT_ERROR_PAGE);
	EXPECT_EQ(server1.client_body_limit, DEFAULT_CLIENT_BODY_LIMIT);

	ASSERT_EQ(server1.routes.size(), 1);
	const RouteConfig& route1 = server1.routes.at(DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.route, DEFAULT_ROUTE_PATH);
	EXPECT_EQ(route1.redirection, DEFAULT_REDIRECTION);
	EXPECT_EQ(route1.root_directory, DEFAULT_ROOT_DIRECTORY);
	EXPECT_EQ(route1.accepted_methods.size(), 2);
	EXPECT_EQ(route1.accepted_methods[0], "GET");
	EXPECT_EQ(route1.accepted_methods[1], "DELETE");
	EXPECT_EQ(route1.directory_listing, DEFAULT_DIRECTORY_LISTING);
	EXPECT_EQ(route1.default_file, DEFAULT_FILE);

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

TEST(ConfigTest, TwoRoutes) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/TwoRoutes.conf");
	ASSERT_EQ(config->servers.size(), 1);

	const ServerConfig& server = config->servers[0];

	ASSERT_EQ(server.routes.size(), 2);
	const RouteConfig& route1 = server.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");

	const RouteConfig& route2 = server.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.redirection, "/route_y_redirection");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");
}

TEST(ConfigTest, ThreeRoutes) {
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/ThreeRoutes.conf");
	ASSERT_EQ(config->servers.size(), 1);

	const ServerConfig& server = config->servers[0];

	ASSERT_EQ(server.routes.size(), 3);
	const RouteConfig& route1 = server.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");

	const RouteConfig& route2 = server.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.redirection, "/route_y_redirection");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");

	const RouteConfig& route3 = server.routes.at("/route_z");
	EXPECT_EQ(route3.route, "/route_z");
	EXPECT_EQ(route3.redirection, "/route_z_redirection");
	EXPECT_EQ(route3.root_directory, "/var/www/route_z");
}


TEST(ConfigTest, TwoServersTwoRoutes)
{
	Config  *config = Config::getInstance();

	config->loadConfig("./test/conf_test_files/TwoServersTwoRoutes.conf");
	printConfig();
	ASSERT_EQ(config->servers.size(), 2);

	const ServerConfig& server = config->servers[0];

	ASSERT_EQ(server.routes.size(), 2);
	const RouteConfig& route1 = server.routes.at("/route_x");
	EXPECT_EQ(route1.route, "/route_x");
	EXPECT_EQ(route1.redirection, "/route_x_redirection");
	EXPECT_EQ(route1.root_directory, "/var/www/route_x");

	const RouteConfig& route2 = server.routes.at("/route_y");
	EXPECT_EQ(route2.route, "/route_y");
	EXPECT_EQ(route2.redirection, "/route_y_redirection");
	EXPECT_EQ(route2.root_directory, "/var/www/route_y");

	const ServerConfig& server2 = config->servers[1];

	ASSERT_EQ(server2.routes.size(), 2);
	const RouteConfig& route3 = server2.routes.at("/route_z");
	EXPECT_EQ(route3.route, "/route_z");
	EXPECT_EQ(route3.redirection, "/route_z_redirection");
	EXPECT_EQ(route3.root_directory, "/var/www/route_z");

	const RouteConfig& route4 = server2.routes.at("/route_w");
	EXPECT_EQ(route4.route, "/route_w");
	EXPECT_EQ(route4.redirection, "/route_w_redirection");
	EXPECT_EQ(route4.root_directory, "/var/www/route_w");
}


