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

- off/on directory listing
