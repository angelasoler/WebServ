#include <sys/epoll.h>
#include <signal.h>
#include <unistd.h>
#include <vector>
#include <iostream>

volatile sig_atomic_t keep_running = 1;

void handle_signal(int signal) {
    keep_running = 0;
}

void setup_signal_handlers() {
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
}

class Server {
public:
    void run();

private:
    int epoll_fd;
    int server_fd;
    std::vector<struct epoll_event> events;
    void handleError(const char* msg);
    void acceptConnection();
    bool handleClient(size_t index);
    void answer_it(size_t index);
    int getActionNumber(const std::string& msg);
};

void Server::run()
{
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        handleError("epoll_create1 failed");
        return;
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        handleError("epoll_ctl failed");
        return;
    }

    events.resize(16);
    setup_signal_handlers();

    while (keep_running)
    {
        int event_count = epoll_wait(epoll_fd, events.data(), events.size(), -1);
        if (event_count < 0)
        {
            if (errno == EINTR) {
                // epoll_wait foi interrompido por um sinal, verifica a flag e possivelmente termina o loop
                continue;
            }
            handleError("epoll_wait failed");
            break;
        }

        for (int i = 0; i < event_count; ++i)
        {
            if (events[i].events & EPOLLIN)
            {
                if (events[i].data.fd == server_fd)
                {
                    acceptConnection();
                }
                else
                {
                    if (handleClient(i))
                    {
                        // handleClient irá atualizar o evento para EPOLLOUT se necessário
                    }
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                // Lógica de escrita
            }
        }
    }

    close(epoll_fd);
}