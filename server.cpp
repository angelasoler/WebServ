#include "server.hpp"

// Construtor
Server::Server(int port)
{
	// Configuração do endereço do servidor
	address.sin_addr.s_addr = INADDR_ANY;	// Aceita conexões de qualquer endereço IP disponível
	address.sin_port = htons(port);			// htons() é uma função que converte o número da porta do formato host para o formato de rede.
	address.sin_family = AF_INET;			// Definição de protocolo IPv4
	addrlen = sizeof(address);				// Tamanho da estrutura de endereço
	createSocket();							// Cria o socket do servidor
	configureSocket();				// Configura o socket
	bindSocket();					// Liga o socket ao endereço e porta especificados
	listenSocket();					// Coloca o socket em estado de escuta por conexões
}

// Destrutor
Server::~Server(void)
{
	while (poll_fds.size())
	{
		close(poll_fds.begin()->fd);
		poll_fds.erase(poll_fds.begin());
	}
}

// Funções auxiliares do construtor

void Server::createSocket(void)
{
	// Criação do socket TCP
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{
		handleError("Socket creation failed");
	}
}


// CONFIGURANDO SOCKET
// A principal finalidade de configurar o socket para reutilizar
// endereços e portas (SO_REUSEADDR | SO_REUSEPORT) é permitir
// que o servidor seja reiniciado e comece a aceitar conexões rapidamente na mesma
// combinação de endereço IP e porta. Isso é particularmente
// útil em ambientes de desenvolvimentoe em servidores
// que precisam ser reiniciados com frequência,
// evitando problemas de "endereço já em uso"
// que ocorreriam se a opção não fosse configurada.
void Server::configureSocket(void)
{
	// Configura o socket para reutilizar endereços e portas
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void Server::bindSocket(void)
{
	// Liga o socket ao endereço e porta especificados
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void Server::listenSocket(void)
{
	// Coloca o socket em estado de escuta por conexões
	if (listen(server_fd, 3) < 0)
	{
		handleError("Listen failed");
	}
	std::cout << "Pronto para conectar com clientes\nUtilize: '~$ \
	telnet localhost 8080' em outro terminal para testar!" << std::endl;

	// Configuração para uso de poll no socket do servidor
	struct pollfd server_poll_fd;
	server_poll_fd.fd = server_fd;
	server_poll_fd.events = POLLIN | POLLOUT;
	server_poll_fd.revents = 0;
	poll_fds.push_back(server_poll_fd);
}

void	Server::answer_it(size_t i)
{
	send(poll_fds[i].fd, "Hello", 5, 0);
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

int	Server::getActionNumber(std::string text)
{
	if (!std::strncmp(text.c_str(), "GET", 3)
		|| !std::strncmp(text.c_str(), "POST", 4)
		|| !std::strncmp(text.c_str(), "DELETE", 6))
	{
		return (0);
	}
	else if (text.size() > 50)
		return (2);
	else
		return (1);
}

// Função principal para execução do servidor
void Server::run(void)
{
	while (true)
	{
		// Aguarda por eventos nos sockets monitorados pelo poll
		int poll_count = poll(poll_fds.begin().base(), poll_fds.size(), -1);
		if (poll_count < 0)
		{
			handleError("Poll failed");
		}

		// Itera sobre os descritores de arquivo monitorados pelo poll
		for (size_t i = 0, j = poll_fds.size(); i < j; ++i)
		{
			if (poll_fds[i].revents & POLLIN)
			{
				// Se o evento for de entrada no socket do servidor, aceita uma nova conexão
				if (poll_fds[i].fd == server_fd)
				{
					acceptConnection();
				}
				// Caso contrário, trata a leitura de dados de um cliente existente
				else
				{
					if (handleClient(i))
					{
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
						case 0:
							answer_it(i);
							break;
						case 1:
							poll_fds[i].revents = POLLIN;
							break;
						case 2:
							message.erase(message.find(poll_fds[i].fd));
							close(poll_fds[i].fd);
							poll_fds.erase(poll_fds.begin() + i);
							break;
					}
				}
				// Give the proper answer or set Revents to POLLIN (to read again)
				else
					poll_fds[i].revents = POLLIN;
			}
		}
	}
}
// Função para aceitar uma nova conexão de cliente
void Server::acceptConnection(void)
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
	client_poll_fd.revents = POLLHUP;
	poll_fds.push_back(client_poll_fd);	// Adiciona ao vetor de estruturas de poll
}

// Função para tratar dados recebidos de um cliente
int Server::handleClient(size_t i)
{
	int client_fd = poll_fds[i].fd;
	memset(buffer, 0, BUFFER_SIZE + 1);
	ssize_t bytes_read = recv(client_fd, buffer, BUFFER_SIZE, 0);
	if (bytes_read >= 0)
		buffer[bytes_read] = 0;
	// printf("O que foi recebido %p - %s\n", buffer, buffer);
	// for (int i = 0; buffer[i]; i++)
	// 	printf("%d ", buffer[i]);
	if (bytes_read < 0)
	{
		handleError("Read failed");
		// Identify error and properly handle it
	}
	else if (bytes_read == 0)
	{
		std::cout << "Conexão fechada pelo cliente batata" << std::endl;
		if (!message.empty())
		{
			std::map<int, std::string>::iterator ite = message.find(client_fd);
			if (ite != message.end())
				message.erase(ite);
		}
		close(client_fd);
		for (std::vector<struct pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); it++)
		{
			if (it->fd == client_fd)
			{
				poll_fds.erase(it);
				break ;
			}
		}
		return (1);
	}
	else
	{
		message[client_fd] += buffer;
		std::cout << "Recebido do fd " << client_fd << ": " << buffer << std::endl;
	}
	return (0);
}

// Função para tratar erros, exibe mensagem de erro e encerra o programa
void Server::handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
	// this->~Server();
	exit(1);
}

// Função para configurar operação não bloqueante em um socket
int Server::setNonBlocking(int fd)
{
	int flags = fcntl(fd, F_GETFL, 0);	// Obtém flags atuais do descritor de arquivo
	if (flags == -1)
		return -1;	// Em caso de erro, retorna -1
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);	// Configura não bloqueante e retorna resultado
}
