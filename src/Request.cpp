
#include "Request.hpp"
#include "ParsePathInfo.hpp"
#include "ParseBodyInfo.hpp"
#include "RequestReader.hpp"
#include "PrintRequestInfo.hpp"
#include "TimeNow.hpp"
#include <fstream>
#include <cerrno>

Request::Request(void) {}

Request::~Request(void) {}

void	Request::printRequest(void)
{
	std::ofstream	requestLog("logs/request.log", std::ios_base::app);

	requestLog << TimeNow()
				<< std::endl;
	requestLog << requestsText
				<< std::endl;
	requestLog.close();
}

bool isComplete(const std::vector<char>& clientRequestText) {
	// Procurar o fim do cabeçalho HTTP
	const char* headerEnd = "\r\n\r\n";
	std::vector<char>::const_iterator it = std::search(clientRequestText.begin(), clientRequestText.end(), 
													   headerEnd, headerEnd + 4);
	if (it == clientRequestText.end()) {
		// std::cerr << "isComplete: CCCCCCCC " << "\n" << std::endl;
		return true;  // O cabeçalho ainda não foi completamente recebido
	}

	size_t header_end = std::distance(clientRequestText.begin(), it);

	// Procurar a posição do Content-Length no cabeçalho
	const char* contentLengthStr = "Content-Length: ";
	it = std::search(clientRequestText.begin(), clientRequestText.end(), 
					 contentLengthStr, contentLengthStr + 16);
	if (it != clientRequestText.end()) {
		size_t start_pos = std::distance(clientRequestText.begin(), it) + 16;
		it = std::find(clientRequestText.begin() + start_pos, clientRequestText.end(), '\r');
		if (it != clientRequestText.end()) {
			size_t end_pos = std::distance(clientRequestText.begin(), it);
			std::string content_length_str(clientRequestText.begin() + start_pos, clientRequestText.begin() + end_pos);

			size_t content_length = std::atoi(content_length_str.c_str());
			// Verificar se o corpo da requisição foi completamente recebido
			size_t total_length = header_end + 4 + content_length;
			// std::cerr << "isComplete: BBBBBB " << "\n" << std::endl;
			if (clientRequestText.size() >= total_length) {
				return true;  // Cabeçalho e corpo completos
			} else {
				return false;  // Ainda falta parte do corpo
			}
		}
	}

	// std::cerr << "isComplete: AAAAAAAAAA " << "\n" << std::endl;
	RequestReader	requestReader;
	if (requestReader.requestCompleted(clientRequestText))
		return true;

	return false;
}

// bool	Request::readRequest(int client_fd)
// {
// 	requestsText.clear();
// 	info = RequestInfo();
// 	requestReader = RequestReader();
// 	if (!requestReader.readHttpRequest(client_fd))
// 		return (true);
// 	requestsText = requestReader.getFullRequest();
// 	printRequest();
// 	return false;
// }

bool	Request::readRequest(int client_fd)
{
	char buffer[102400];  // Buffer temporário para leitura
	ssize_t bytes_read;

	// Leitura não-bloqueante dos dados do cliente
	while (true) {
		
		bytes_read = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
		buffer[bytes_read] = '\0';  // Garantir terminação de string

		
		if (bytes_read == 0) {
			
			// std::cerr << "readRequest: Client closed connection (fd: " << client_fd << ")" << std::endl;
			info.action = CLOSE;
			break ;
		}

		if (bytes_read < 0) {
			// std::cerr << "readRequest: No data available for now on client fd: " << client_fd << std::endl;
			info.action = AWAIT_READ;
			break ;
		}		

		// std::cerr << "readRequest: Times: " << client_fd << std::endl;
		requestVec.insert(requestVec.end(), buffer, buffer + bytes_read);

		PrintRequestInfo::printVectorChar(requestVec, "Request_Vec", "logs/Request_Vec.log");
		if (isComplete(requestVec)) {
			requestReader = RequestReader();
			requestReader.readHttpRequest(requestVec);
			info.action = RESPONSE;
			break;  // Já temos todos os dados, podemos parar de ler
		}
		memset(buffer, 0, 1024);
	}
	return false;
}

void	adjustRoute(std::string &route)
{
	std::string newRoute;
	bool		appendSlash = false;
	
	newRoute.reserve(route.size());
	for (std::string::iterator it = route.begin(); it != route.end(); it++)
	{
		if (*it == '/')
		{
			if (!appendSlash)
			{
				newRoute += *it;
				appendSlash = true;
			}
		}
		else
		{
			newRoute += *it;
			appendSlash = false;
		}
	}
	route = newRoute;
}

std::vector<std::string> insertAllowedMethods(void)
{
	std::vector<std::string>	vec;

	vec.push_back("GET");
	vec.push_back("POST");
	vec.push_back("DELETE");
	return vec;
}

int	Request::getInformationalStatus(void)
{
	std::vector<std::string> 			allowedMethods = insertAllowedMethods();

	std::vector<std::string>::iterator	requestedMethod;


	if (!requestReader.getHttpVersion().empty() && requestReader.getHttpVersion() != "HTTP/1.1")
		return 400;
	requestedMethod = std::find (allowedMethods.begin(), allowedMethods.end(), requestReader.getMethod());
	if (requestedMethod != allowedMethods.end())
	{
		requestedMethod = std::find(info.routeRef.accepted_methods.begin(),
										info.routeRef.accepted_methods.end(),
										requestReader.getMethod());
		if (requestedMethod != info.routeRef.accepted_methods.end())
		{
			return 200;
		}
		return 405; // Not ALLOWED
	}
	return 400; // Bad
}

void	Request::parseRequestInfo(ServerConfig &serverConfig)
{
	info.action = getMethodAction();
	if (info.action == CLOSE)
		return ;
	info.rawBody = requestReader.getRawBody();
	info.body = requestReader.getBody();
	info.contentType = requestReader.getHeader("Content-Type");
	info.requestedRoute = requestReader.getRequestedRoute();
	adjustRoute(info.requestedRoute);
	info.serverRef = serverConfig;
}

void Request::parseRequest(ServerConfig &serverConfig)
{
	parseRequestInfo(serverConfig);
	ParsePathInfo::parsePathInfo(info);
	int	status = getInformationalStatus();
	if (status != 200)
	{
		info.action = RESPONSE;
		info.pathType = UNKNOWN;
		if (status == 405)
			info.requestedRoute = "";
		else
			info.requestedRoute = "Bad";
		return ;
	}
	ParseBodyInfo::parseBodyInfo(info);
	PrintRequestInfo::printRequestInfo(info);
}

e_httpMethodActions	Request::getMethodAction(void)
{
	// printHeaderDataStructure();
	std::vector<std::string> allowedMethods = insertAllowedMethods();

	std::vector<std::string>::iterator	requestedMethod;

	requestedMethod = std::find (allowedMethods.begin(), allowedMethods.end(), requestReader.getMethod());
	if (requestedMethod != allowedMethods.end())
	{
		std::cout << "\t" << *requestedMethod << "\n";
		return ((e_httpMethodActions)(requestedMethod - allowedMethods.begin()));
	}
	if (!requestReader.getMethod().empty())
		return (RESPONSE);
	std::cout << "\tCLOSE\n" << std::endl;
	return(CLOSE);
}

RequestInfo::RequestInfo() :
	requestedRoute("\\"),
	fullPath(""),
	pathType(UNKNOWN),
	permissions(),
	serverRef(),
	routeRef(),
	contentType(""),
	action(RESPONSE),
	body(""),
	multipartHeaders(),
	multipartValues(),
	urlencodedValues()
	{}