#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handleError(std::string msg)
{
	std::cerr << msg << std::endl;
	exit(1);
}

int main(void)
{
	int 				server_fd;
	int					client_fd;

	// Criar socket do server
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		handleError("socket failed");
	}



	// Forçar o socket a utilizar a porta
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0)
	{
		handleError("setsockopt");
	}
	struct sockaddr_in	address;
	int 				addrlen = sizeof(address);
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// Ligando o socket a porta
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
	{
		handleError("bind failed");
	}

	// Ouvir conexões
	if (listen(server_fd, 3) < 0)
	{
		handleError("listen");
	}


	std::cout << "Pronto pra conectar com cliente\nutilize : '~$ telnet localhost 8080' em outro terminal para testar!" << std::endl;
	// Aceitar a conexão de entrada e criando socket do cliente
	if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("accept");
	}

	// Configurar a estrutura pollfd
	struct pollfd 		poll_fds[1];
	poll_fds[0].fd = client_fd;
	poll_fds[0].events = POLLIN;




	// Loop infinito para ouvir e imprimir dados recebidos
	char 				buffer[BUFFER_SIZE];

	while (true)
	{
		int poll_count = poll(poll_fds, 1, -1);
		if (poll_count < 0)
		{
			handleError("poll");
		}
		if (poll_fds[0].revents & POLLIN)
		{
			memset(buffer, 0, BUFFER_SIZE);
			int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
			if (bytes_read < 0)
			{
				handleError("read");
			}
			else if (bytes_read == 0)
			{
				std::cout << "Connection closed by client" << std::endl;
				break;
			}
			std::cout << "Received: " << buffer << std::endl;
		}
	}

	// Fechar o socket
	close(client_fd);
	close(server_fd);

	return 0;
}