#include <gtest/gtest.h>
#include <cstdlib>
#include <thread>
#include <fstream>


void start_server() {
	std::system("./webserv &");
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

TEST(HttpTest, FirefoxGetRequest) {
	start_server();
	int result = std::system("test/test-firefox");

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

TEST(ConfigTest, LoadConfig) {
    Config config;

	config.loadConfig("config.conf");
	printConfig(config);
	return ;
    ASSERT_EQ(config.servers.size(), 2);

    // Verificação do primeiro servidor
    const ServerConfig& server1 = config.servers[0];
    EXPECT_EQ(server1.host, "localhost");
    EXPECT_EQ(server1.port, 8080);
    EXPECT_EQ(server1.server_names[0], "example_x.com");
    EXPECT_EQ(server1.default_error_page, "/errors/404.html");
    EXPECT_EQ(server1.client_body_limit, 1048576);

    // Verificação da rota do primeiro servidor
    ASSERT_EQ(server1.routes.size(), 1);
    const RouteConfig& route1 = server1.routes.at("/route_x");
    EXPECT_EQ(route1.path, "/route_x");
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
    const ServerConfig& server2 = config.servers[1];
    EXPECT_EQ(server2.host, "192.168.1.1");
    EXPECT_EQ(server2.port, 9090);
    EXPECT_EQ(server2.server_names[0], "example_y.com");
    EXPECT_EQ(server2.default_error_page, "/errors/500.html");
    EXPECT_EQ(server2.client_body_limit, 2097152);

    // Verificação da rota do segundo servidor
    ASSERT_EQ(server2.routes.size(), 1);
    const RouteConfig& route2 = server2.routes.at("/route_y");
    EXPECT_EQ(route2.path, "/route_y");
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

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
