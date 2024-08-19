#include <iostream>
#include <ctime>

std::string	TimeNow(void) {
	std::time_t	now = time(0);
	std::string	ret;

	char* dt = ctime(&now);
	ret = dt;
	return ret;
}
