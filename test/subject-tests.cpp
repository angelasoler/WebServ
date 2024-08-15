#include <gtest/gtest.h>
#include <curl/curl.h>

void start_server();
void stop_server();

- um servido pode subir sem server_name

-  The first server for a host:port will be the default for this host:port (that means
it will answer to all the requests that don’t belong to an other server).??

- se o config tiver o default error pages, o mesmo deve ser usado

- o tamanho do body deve ser respeito deacordo ao config-file(POST)

// as rotas podem ter uma ou mais das seguentes regras:
- só deve executar os petodos configurados

- executa redirections se configurado

- se um pasta está definida ela será a root onde será procura,
 senão, procura na root do server?

- se directory listing está on renderizar html com links para os conteudos do diretorio

- se está off retornar default file (e se não tiver default file? eu faço)

- cgi roda com python, aceita GET e POST

- o cgi-script se encontra na pasta cgi-bin e qualquer coisa que use path relativos deve funcionar
    `The CGI should be run in the correct directory for relative path file access.`

