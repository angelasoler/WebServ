
#include "tests.hpp"

void start_server(std::string configFile) {
	std::string	binaryName = "./webserv";
	std::string	logFileRedirection = " > logs/server-outputs.log";
	std::string	ampersandOp = "&";
	std::string	finalExec = binaryName + " " + configFile + logFileRedirection + ampersandOp;

	if (std::system(finalExec.c_str()))
		std::cerr << "Erro: " << finalExec << std::endl;
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
	// ARRANGE
	start_server("");
	// ACT
	int result = std::system("test/test-firefox");

	// ASSERT
	EXPECT_EQ(result, 0);
	stop_server();
}

TEST(HttpTest, TelnetGetRequest) {
	start_server("");
	std::string response = exec("test/test-telnet");

	EXPECT_NE(response.find("Hello"), std::string::npos);
	stop_server();
}


int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
