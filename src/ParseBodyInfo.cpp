#include "ParseBodyInfo.hpp"
#include <sys/stat.h>
#include <string.h>

// void parseRequest(const std::string& requestText, RequestInfo& requestInfo);

std::string extractBoundary(const std::string& contentType) {
    std::string boundary;
    size_t pos = contentType.find("boundary=");
    if (pos != std::string::npos) {
        boundary = contentType.substr(pos + 9);
    }
    return "--" + boundary;
}

void handleMultipartData(const std::string& boundary, const std::string& body) {
    size_t pos = 0;
    std::string partBoundary = boundary + "\r\n";
    while ((pos = body.find(partBoundary)) != std::string::npos) {
        size_t endPos = body.find(boundary, pos + partBoundary.length());
        if (endPos == std::string::npos) break;
        
        std::string part = body.substr(pos + partBoundary.length(), endPos - pos - partBoundary.length());
        
        // Processar cabeçalhos da parte
        size_t headerEndPos = part.find("\r\n\r\n");
        std::string headers = part.substr(0, headerEndPos);
        std::string content = part.substr(headerEndPos + 4);
        
        // Verifica se esta parte é um arquivo
        if (headers.find("filename=") != std::string::npos) {
            // Extrai o nome do arquivo
            size_t filenamePos = headers.find("filename=");
            size_t filenameStart = headers.find("\"", filenamePos) + 1;
            size_t filenameEnd = headers.find("\"", filenameStart);
            std::string filename = headers.substr(filenameStart, filenameEnd - filenameStart);

            // Salva o conteúdo do arquivo
            // std::ofstream outFile(filename, std::ios::binary);
            // outFile.write(content.c_str(), content.length());
            // outFile.close();
            std::cout << "Arquivo salvo: " << filename << std::endl;
        }
        
        pos = endPos;
    }
}

std::string getContentAfterNull(const std::string& str, int n) {
    int nullCount = 0;
    size_t pos = 0;

    // Iterar pela string procurando pelo n-ésimo \0
    while (pos < str.size() && nullCount < n) {
        if (str[pos] == '\0') {
            nullCount++;
        }
        pos++;
    }

    // Se encontrarmos o n-ésimo \0, retornamos o conteúdo após ele
    if (nullCount == n) {
        return std::string(str.c_str() + pos);
    }

    // Se o n-ésimo \0 não for encontrado, retornamos uma string vazia
    return "";
}

void ParseBodyInfo::parseBodyInfo(std::string requestText, RequestInfo &info)
{	
	(void)requestText;
	(void)info;
	// info.body = requestText;
	// parseRequest(requestText, info);
    // std::istringstream requestStream(requestText);
    // std::string line;

    // while (std::getline(requestStream, line))
	// std::cout << "Request:  " << requestText << " \n";
    // size_t contentTypePos = requestText.find("Content-Type: ");
    // size_t contentTypeEnd = requestText.find("\r\n", contentTypePos);
    // std::string contentType = requestText.substr(contentTypePos + 14, contentTypeEnd - contentTypePos - 14);

    // // Extrair o boundary
    // std::string boundary = extractBoundary(contentType);
    // std::cout << "Boundary: " << boundary << std::endl;

    // // Processar o corpo da requisição
    // size_t bodyPos = requestText.find("\\r\\n\\r\\n");
    // std::string body = requestText.substr(bodyPos + 4);
    // info.body = getContentAfterNull(requestText, 5);
    std::cout << info.body << "//OK\n";
    // handleMultipartData(boundary, body);

    // Resposta HTTP
    // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nUpload de arquivo processado com sucesso!";
    // send(new_socket, response.c_str(), response.length(), 0);
    // std::cout << "Resposta enviada\n";

    // close(new_socket);
    // close(server_fd);

    // return 0;
}


// Função auxiliar para decodificar URL
std::string urlDecode(const std::string& encoded) {
    std::string decoded;
    char hex[3] = {0};
    for (size_t i = 0; i < encoded.length(); ++i) {
        if (encoded[i] == '%') {
            hex[0] = encoded[i + 1];
            hex[1] = encoded[i + 2];
            decoded += static_cast<char>(strtol(hex, NULL, 16));
            i += 2;
        } else if (encoded[i] == '+') {
            decoded += ' ';
        } else {
            decoded += encoded[i];
        }
    }
    return decoded;
}

// void parseRequest(const std::string& requestText, RequestInfo& requestInfo) {
// 	std::istringstream requestStream(requestText);
// 	std::string line;
	
// 	(void)requestText;
// 	(void)requestInfo;
// 	std::cout << requestText.find("\n\0\n\0") << "\n";
// 	// // Parse header lines
// 	// while (std::getline(requestStream, line) && !line.empty()) {
// 	// 	// Remove '\r' at the end of each line if it exists
// 	// 	if (!line.empty() && line[line.size() - 1] == '\r') {
// 	// 		line.erase(line.size() - 1);
// 	// 	}

// 	// 	// Check for Content-Type header
// 	// 	if (line.find("Content-Type:") != std::string::npos) {
// 	// 		requestInfo.contentType = line.substr(14); // 14 is the length of "Content-Type: "
// 	// 	}
// 	// }
	
// 	// Parse the body if it exists
// 	// if (std::getline(requestStream, line)) {
// 	// 	requestInfo.body = line;
		
// 	// 	// If Content-Type is application/x-www-form-urlencoded, parse the body into the map
// 	// 	if (requestInfo.contentType == "application/x-www-form-urlencoded") {
// 	// 		std::istringstream bodyStream(requestInfo.body);
// 	// 		std::string keyValuePair;
			
// 	// 		while (std::getline(bodyStream, keyValuePair, '&')) {
// 	// 			size_t pos = keyValuePair.find('=');
// 	// 			if (pos != std::string::npos) {
// 	// 				std::string key = keyValuePair.substr(0, pos);
// 	// 				std::string value = keyValuePair.substr(pos + 1);
// 	// 				requestInfo.requestBody[key] = value;
// 	// 			}
// 	// 		}
// 	// 	}
// 	// }
// }
