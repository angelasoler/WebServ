#include "server.hpp"
int readNTimes = 0;

Server::Server(int port)
{
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	addrlen = sizeof(address); //retirar addrlen?
	createSocket();
	configureSocket();
	bindSocket();
	listenSocket();
}

Server::~Server(void)
{
	while (poll_fds.size())
	{
		close(poll_fds.begin()->fd);
		poll_fds.erase(poll_fds.begin());
	}
}

void	Server::createSocket(void)
{
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		handleError("Socket creation failed");
	}
}

void	Server::configureSocket(void)
{
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void	Server::bindSocket(void)
{
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void	Server::listenSocket(void)
{
	if (listen(server_fd, 3) < 0)
	{
		handleError("Listen failed");
	}
	std::cout << "Pronto para conectar com clientes\nUtilize: '~$ \
	telnet localhost 8080' em outro terminal para testar!" << std::endl;

	struct pollfd server_poll_fd;
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN | POLLOUT;
	// server_poll_fd.revents = 0;
	poll_fds.push_back(server_poll_fd);
}

void	Server::answer_it(size_t i)
{
	std::cout << i << std::endl;
	send(poll_fds[i].fd, "Hello\n", 5, 0);
	close(poll_fds[i].fd);
	message.erase(message.find(poll_fds[i].fd));
	for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); it++)
	{
		if (it->fd == poll_fds[i].fd)
		{
			poll_fds.erase(it);
			break;
		}
	}
}

int		Server::getActionNumber(std::string text)
{
	if (!std::strncmp(text.c_str(), "GET", 3)
		|| !std::strncmp(text.c_str(), "POST", 4)
		|| !std::strncmp(text.c_str(), "DELETE", 6))
	{
		std::cout << "RESPONSE" << std::endl;
		return (RESPONSE);
	}
	else if (text.size() > 50) {
		std::cout << "CLOSE" << std::endl;
		return (CLOSE);
	}
}

void	Server::run(void)
{
	while (true)
	{
		int poll_count = poll(poll_fds.begin().base(), poll_fds.size(), -1);
		if (poll_count < 0)
		{
			handleError("Poll failed");
		}

		for (size_t i = 0; i < poll_fds.size(); ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				std::cout << "polls:  " << poll_fds.size() << std::endl;
				// Se o evento for de entrada no socket do servidor, aceita uma nova conexão
				if (poll_fds[i].fd == server_fd)
				{
					std::cout << "********** acceptConnection *************" << std::endl;
					std::cout << "waiting " << i << std::endl;
					acceptConnection();
				}
				// Caso contrário, trata a leitura de dados de um cliente existente
				else
				{
					if (handleClient(i))
					{
						std::cout << "********** handleClient TRUE *************" << std::endl;
						std::cout << "waiting" << i << std::endl;
						// answer_it(i);
					}
				}
			}
			else if (poll_fds[i].revents & POLLOUT)
			{
				// Get all text
				// analyze it [For this, check the start word, then you analyze it profoundly]
				if (!message.find(poll_fds[i].fd)->second.empty())
				{
					switch (getActionNumber(message.find(poll_fds[i].fd)->second))
					{
						std::cout << "****** getActionNumber *****" << std::endl;
						case RESPONSE:
							answer_it(i);
							std::cout << "****** readNTimes " << readNTimes << "*****" << std::endl;
							readNTimes = 0;
							break;
						case REVENT:
							// poll_fds[i].revents = POLLIN;
							break;
						case CLOSE:
							//just closing for telnet
							message.erase(message.find(poll_fds[i].fd));
							close(poll_fds[i].fd);
							poll_fds.erase(poll_fds.begin() + i);
							break;
					}
				}
				// Give the proper answer or set Revents to POLLIN (to read again)
				else {
					readNTimes++;
					// poll_fds[i].revents = POLLIN;
				}
			}
		}
	}
}

void	Server::acceptConnection(void)
{
	int new_socket;
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	// Mensagem informativa sobre a nova conexão
	std::cout << "Nova conexão, socket fd é " << new_socket << ", ip é: " << ((address.sin_addr.s_addr << 24) >> 24)
		<< "." << ((address.sin_addr.s_addr << 16) >> 24)
		<< "." << ((address.sin_addr.s_addr << 8) >> 24)
		<< "." << ((address.sin_addr.s_addr << 0) >> 24) << ", porta: " << address.sin_port << std::endl;

	// Configura o novo socket para operação não bloqueante
	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}

	// Configura o novo socket para monitoramento por poll
	struct pollfd client_poll_fd;
	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN | POLLOUT;
	// client_poll_fd.revents = POLLHUP;
	poll_fds.push_back(client_poll_fd);	// Adiciona ao vetor de estruturas de poll
}


int		Server::handleClient(size_t i) // int		Server::handleClient(int client_fd)
{
	int client_fd = poll_fds[i].fd; //comentar
	memset(buffer, 0, BUFFER_SIZE + 1);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = 0;
	if (bytes_read < 0)
	{
		handleError("Read failed");
	}
	else
	{
		message[client_fd] += buffer;
		std::cout << "Recebido do fd " << client_fd << ":"
		<< buffer 
		<< std::endl;
	}
	std::cout << "********** handleClient FLASE *************" << std::endl;
	return (0);
}

void	Server::handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	// this->~Server();
	exit(1);
}

int		Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);	// Obtém flags atuais do descritor de arquivo
	if (flags == -1)
		return -1;	// Em caso de erro, retorna -1
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);	// Configura não bloqueante e retorna resultado
}
