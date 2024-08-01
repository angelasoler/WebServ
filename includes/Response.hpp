#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include "Request.hpp"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
# include "Server.hpp"
# include "Config.hpp"
# define DEFAULT_BODY "<html><head><title>Teste de Pagina</title></head><body><h1>Bom Dia!</h1></body></html>"
# define NOT_FOUND_PAGE_ERROR "<html><head><title>ERROR</title></head><body><h1>Page not found!</h1></body></html>"

class Response
{
	private:
		std::string statusLine;
		std::map<std::string, std::string> headers;
		std::string body;

	public:
		Response(void);
		~Response(void);

		void setStatusLine(const std::string& version, int statusCode, const std::string& reasonPhrase);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& bodyFile);
		std::string buildResponse();

		void sendResponse(int client_fd);
		int treatActionAndResponse(std::map<int, std::string> request, int client_fd, e_httpMethodActions action);

		void	responseGET(ServerConfig &server, int client_fd);
};

#endif /* RESPONSE_HPP */
