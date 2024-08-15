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
- só deve executar os metodos configurados

- executa redirections se configurado

- se uma pasta está definida ela será a root onde serão procurados os querys,
 senão, procura na root do server

- se directory listing está on renderizar html com links para os conteudos do diretorio

- se está off retornar default file (e se não tiver default file usa default file do webserver)

- cgi roda com python, aceita GET e POST
    -cgi em post pode receber argumentos para o fomulario que serão parseados da query

-  path relativos do cgi-script devem funcionar
    `The CGI should be run in the correct directory for relative path file access.`

////// não tão prioritario pois na testa na regua /////
- coisas obrigatoria de http/1.1

Linha de Requisição (Request Line)

Método HTTP: Deve ser especificado (ex.: GET, POST, DELETE, etc.).
URI: O caminho do recurso solicitado deve ser incluído (ex.: /index.html).
Versão do Protocolo: Deve ser HTTP/1.1.

Host: Deve ser sempre incluído para identificar o host e, opcionalmente, a porta (ex.: Host: www.example.com). Isso é obrigatório em HTTP/1.1, diferentemente do HTTP/1.0.
Outros cabeçalhos podem ser incluídos conforme necessário, mas não são obrigatórios:

Após os cabeçalhos de requisição, deve haver uma linha em branco (um Enter) para separar os cabeçalhos do corpo da requisição, se existir.
Corpo da Requisição (Request Body) (POST)

O corpo não é obrigatório para métodos como GET e DELETE.
////////////////////