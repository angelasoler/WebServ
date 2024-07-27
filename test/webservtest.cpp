#include <gtest/gtest.h>
#include <cstdlib>
#include <thread>


void start_server() {
	std::system("./webserv &");
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void stop_server() {
	std::system("pkill --signal SIGINT webserv");
}

TEST(HttpTest, GetRequest) {
	start_server();
	int result = std::system("../test/selenium_script.py");
	EXPECT_EQ(result, 0);
	stop_server();
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// TO-DO
// [_]Fazer roda com telnet
// [_] mata marionete do firefox