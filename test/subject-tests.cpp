#include <gtest/gtest.h>
#include "tests.hpp"

//////
// - se o config tiver o default error pages, o mesmo deve ser usado
//////
TEST(SubjectTests, DefaultErrorPageConfig) {
	// ARRANGE: Configuração do teste e inicialização
	////
	std::ofstream	defaultErrorFile("serverRoot/404.html");
	defaultErrorFile << "<html><head><title>Error Title</title>"
					<< "</head><body><h1>sorry babe</h1></body></html>";
	defaultErrorFile.close();
	////
	HttpResponse response;
	CURL* curl;
	start_server("test/subject-config-file/DefaultErrorPage.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/non_existing_path");
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
	EXPECT_TRUE(response.body.find("sorry babe") != std::string::npos);
	//pegar title
	EXPECT_TRUE(response.body.find("Error Title") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}
//////
// - o tamanho do body deve ser respeitado deacordo ao config-file(POST)
//////
TEST(SubjectTests, BodyLimitSize) {
	// ARRANGE:
	std::ofstream	toLongBody("serverRoot/body_limit.html");
	toLongBody << "<html><head><title>Large body</title>"
					<<"</head><body><h1>"
					<< "Lorem ipsum dolor sit amet, consectetur "
					<< "adipiscing elit. Vivamus lacinia odio "
					<< "vitae vestibulum vestibulum. Cras venenatis "
					<< "euismod malesuada. Maecenas non laoreet odio. "
					<< "Fusce nec augue at magna ornare facilisis. "
					<< "Sed ultricies sapien nec urna fringilla, "
					<< "et posuere eros hendrerit. Praesent sed eros "
					<< "quis sapien facilisis lacinia at eget odio. "
					<< "In eget massa vel justo tempor pretium."
					<< "Nulla facilisi. Cras vehicula magna sit amet "
					<< "consectetur condimentum."
					<<"</h1></body></html>";
	toLongBody.close();
	HttpResponse response;
	CURL* curl;
	start_server("test/subject-config-file/bodyLimitSize.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);
	curl_mime *form = curl_mime_init(curl);
	curl_mimepart *field = curl_mime_addpart(form);
	curl_mime_name(field, "file");
	curl_mime_filedata(field, "serverRoot/body_limit.html");
	////

	// ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/cgi-bin/upload/upload.py");
	curl_easy_setopt(curl, CURLOPT_MIMEPOST, form);
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


	// ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 413);
	EXPECT_TRUE(response.body.find("Payload Too Large!") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

//////
// ***** as rotas podem ter uma ou mais das seguentes regras:
//////


//////
// - só deve executar os metodos configurados
// 405 Method Not Allowed
/////
TEST(SubjectTests, MethodNotAllowed) {
	// ARRANGE: Configuração do teste e inicialização
	HttpResponse response;
	CURL* curl;
	start_server("");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	 // ACT: Executar a ação sob teste
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080");
	curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
	curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);


	 // ASSERT: Verificar os resultados
	EXPECT_EQ(res, CURLE_OK);
	EXPECT_EQ(response.status_code, 405);

	curl_easy_cleanup(curl);
	stop_server();
}

//////
// - executa redirections se configurado
// RENAN
//////

//////
// route
// Define a directory or a file from where the file should be searched (for example,
// if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
// /tmp/www/pouic/toto/pouet).
//////

TEST(SubjectTests, routeTest) {
	//ARRANGE
	HttpResponse response;
	CURL* curl;
	start_server("test/routes_test/subjectRouteCase.conf");
	curl = curl_easy_init();
	ASSERT_NE(curl, nullptr);

	// ACT
	curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/kapouet/pouic/toto/pouet");
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
	EXPECT_TRUE(response.body.find("rubber duck") != std::string::npos);

	curl_easy_cleanup(curl);
	stop_server();
}

//////
// - se directory listing está on: renderizar html com links para os conteudo
//                  - se está off: retornar default file (e se não tiver default file usa default file do webserver)
//////


//////
// - cgi roda com python, aceita GET e POST
//     -cgi em post pode receber argumentos para o fomulario que serão parseados do body
//////


//////
// - path relativos do cgi-script devem funcionar
//     `The CGI should be run in the correct directory for relative path file access.`
//////


