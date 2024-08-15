#include <gtest/gtest.h>
#include <curl/curl.h>

void start_server();
void stop_server();

- verificar os explicitamente os restornos de recv e send, 0 ou -1
- remove client if theres an error on recv or send