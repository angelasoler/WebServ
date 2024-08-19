#ifndef TEST_H
#define TEST_H

#include <gtest/gtest.h>
#include <curl/curl.h>
#include <fstream>
#include <cstdlib>
#include <thread>
# include "Request.hpp"

struct HttpResponse {
	long status_code;
	std::map<std::string, std::string> headers;
	std::string body;
};

void		start_server(std::string configFile);
void		stop_server();
size_t		HeaderCallback(char* buffer, size_t size, size_t nitems, void* userdata);
size_t		WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);
RequestInfo	parseHttpRequest(const std::string& httpRequest);

#endif
