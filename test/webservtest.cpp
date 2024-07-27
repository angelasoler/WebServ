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

TEST(HttpTest, GetRequest) {
	start_server();
	int result = std::system("../test/test-firefox");
	std::string response = exec("../test/test-telnet");

	EXPECT_EQ(result, 0);
	std::cout << response << std::endl;
	EXPECT_NE(response.find("Hello"), std::string::npos);
	stop_server();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
