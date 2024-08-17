#include <gtest/gtest.h>
#include "tests.hpp"

//um servidor pode subir sem server_name
TEST(SubjectTests, ServerWithNoName) {
	// ARRANGE: Configuração do teste e inicialização
	HttpResponse response;
	CURL* curl;
	start_server("path_rest_files/NoNameServer.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	 // ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9000");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


	 // ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 200);
	EXPECT_TRUE(response.body.find("Hello, World!") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

// nonsense?
// -  The first server for a host:port will be the default for this host:port (that means
// it will answer to all the requests that don’t belong to an other server).??

// TODO: portas < a 1024 não podem ser utilizadas

// - se o config tiver o default error pages, o mesmo deve ser usado
TEST(SubjectTests, DefaultErrorPageConfig) {
	// ARRANGE: Configuração do teste e inicialização
	std::string bodyDefaultErrorPage = "<h1>This is a default error page</h1>";
	////
	// "<html><head><title>This is a default error page</title></head>"
	// "<body><h1>This is a default error page</h1></body></html>"
	////
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:9000/non_existing_path");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "GET");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 404);
	EXPECT_TRUE(response.body == defaultErrorPage);
	//pegar title
	EXPECT_TRUE(response.tile == "This is a default error page");

	curl_easy_cleanup(curl);
	stop_server();
}

- o tamanho do body deve ser respeitado deacordo ao config-file(POST)

// as rotas podem ter uma ou mais das seguentes regras:
- só deve executar os metodos configurados

- executa redirections se configurado

- se uma pasta está definida ela será a root onde serão procurados os querys,
 senão, procura na root default

- se directory listing está on: renderizar html com links para os conteudo
                 - se está off: retornar default file (e se não tiver default file usa default file do webserver)

- cgi roda com python, aceita GET e POST
    -cgi em post pode receber argumentos para o fomulario que serão parseados da query

- path relativos do cgi-script devem funcionar
    `The CGI should be run in the correct directory for relative path file access.`
