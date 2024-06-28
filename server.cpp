#include "server.hpp"

// Construtor
Server::Server(int port)
{
	// Configuração do endereço do servidor
	address.sin_addr.s_addr = INADDR_ANY;	// Aceita conexões de qualquer endereço IP disponível
	address.sin_port = htons(port);			// htons() é uma função que converte o número da porta do formato host para o formato de rede.
	addrlen = sizeof(address);				// Tamanho da estrutura de endereço
	createSocket();							// Cria o socket do servidor
	configureSocket();				// Configura o socket
	bindSocket();					// Liga o socket ao endereço e porta especificados
	listenSocket();					// Coloca o socket em estado de escuta por conexões
}

// Destrutor
Server::~Server()
{
	close(server_fd);
}

// Funções auxiliares do construtor

void Server::createSocket()
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
void Server::configureSocket()
{
	// Configura o socket para reutilizar endereços e portas
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
	{
		handleError("Setsockopt failed");
	}
}

void Server::bindSocket()
{
	// Liga o socket ao endereço e porta especificados
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
	{
		handleError("Bind failed");
	}
}

void Server::listenSocket()
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
	server_poll_fd.events = POLLIN;
	poll_fds.push_back(server_poll_fd);
}

// Função principal para execução do servidor
void Server::run()
{
	while (true)
	{
		// Aguarda por eventos nos sockets monitorados pelo poll
		int poll_count = poll(poll_fds.data(), poll_fds.size(), -1);
		if (poll_count < 0)
		{
			handleError("Poll failed");
		}

		// Itera sobre os descritores de arquivo monitorados pelo poll
		for (size_t i = 0; i < poll_fds.size(); ++i)
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
					handleClient(i);
				}
			}
		}
	}
}

// Função para aceitar uma nova conexão de cliente
void Server::acceptConnection()
{
	int new_socket;
	if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
	{
		handleError("Accept failed");
	}

	// Mensagem informativa sobre a nova conexão
	std::cout << "Nova conexão, socket fd é " << new_socket << ", ip é: " << inet_ntoa(address.sin_addr) << ", porta: " << ntohs(address.sin_port) << std::endl;

	// Configura o novo socket para operação não bloqueante
	if (setNonBlocking(new_socket) < 0)
	{
		handleError("Set non-blocking failed");
	}

	// Configura o novo socket para monitoramento por poll
	struct pollfd client_poll_fd;
	client_poll_fd.fd = new_socket;
	client_poll_fd.events = POLLIN;
	poll_fds.push_back(client_poll_fd);	// Adiciona ao vetor de estruturas de poll
}

// Função para tratar dados recebidos de um cliente
void Server::handleClient(size_t i)
{
	int client_fd = poll_fds[i].fd;
	memset(buffer, 0, BUFFER_SIZE);
	int bytes_read = read(client_fd, buffer, BUFFER_SIZE);
	if (bytes_read < 0)
	{
		handleError("Read failed");
	}
	else if (bytes_read == 0)
	{
		std::cout << "Conexão fechada pelo cliente" << std::endl;
		close(client_fd);
		poll_fds.erase(poll_fds.begin() + i);
	}
	else
	{
		std::cout << "Recebido do fd " << client_fd << ": " << buffer << std::endl;
	}
}

// Função para tratar erros, exibe mensagem de erro e encerra o programa
void Server::handleError(const std::string& msg)
{
	std::cerr << msg << std::endl;
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
