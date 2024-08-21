#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

void parseBodyValues(RequestInfo &info);
void extractBody(std::string requestText, RequestInfo &info);
std::string stringWithNewDivisor(std::string text, std::string newDivisor);

void ParseBodyInfo::parseBodyInfo(std::string requestText, RequestInfo &info)
{	
	if (info.action != UPLOAD)
		return;
	extractBody(requestText, info);
	const std::string contentTypeHeader = "Content-Type: application/x-www-form-urlencoded\r\n";
	if (requestText.find(contentTypeHeader) != std::string::npos)
		parseBodyValues(info);
}

void extractBody(std::string requestText, RequestInfo &info)
{
	// Substitue todo \r\n por \n
	std::string newDivisor = "\n";
	info.body = stringWithNewDivisor(requestText, newDivisor);

	// \r\n\r\n se é representada por \n\n
	std::string finalDivisor = newDivisor + newDivisor;


	// tenta encontrar o equivalente a \r\n\r\n no texto, ou seja tenta encontrar \n\n
	std::size_t pos = info.body.find(finalDivisor);
	if (pos != std::string::npos) {
		// Verifica se há algo após "\r\n\r\n"
		if (pos + finalDivisor.size() < info.body.size()) {
			std::string posDivisor = info.body.substr(pos + finalDivisor.size());
			info.body = posDivisor;
			// std::cout << info.body;
		}
		else {
			// "Não há nada após a sequência \\r\\n\\r\\n.
		}
	}
	else {
		// "A sequência \\r\\n\\r\\n não foi encontrada no texto
	}
}

void parseBodyValues(RequestInfo &info)
{
	(void)info;
}

std::string stringWithNewDivisor(std::string text, std::string newDivisor)
{
	std::string result;

	size_t pos = 0;
	size_t oldDivisorPos;
	
	while ((oldDivisorPos = text.find("\r\0", pos)) != std::string::npos)
	{
		result.append(text, pos, oldDivisorPos - pos);
		result.append(newDivisor);
		pos = oldDivisorPos + 2;
	}
	result.append(text, pos);
	return result;
}
