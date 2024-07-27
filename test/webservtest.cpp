#include <gtest/gtest.h>
#include <cstdlib>
#include <thread>


void start_server() {
	std::system("./webserv &");
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

// TO-DO
// [_]Fazer roda com telnet
// [_]matar processo do webserv antes de terminar
TEST(HttpTest, GetRequest) {
	start_server();
	int result = std::system("../test/selenium_script.py");
	std::cout << "\n\n" << result << "\n\n" << std::endl;
	EXPECT_EQ(result, 0);
}

int main(int argc, char **argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

