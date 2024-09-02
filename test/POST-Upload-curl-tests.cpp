#include "tests.hpp"
#include "Config.hpp"

TEST(POSTuploadFile, UploadMakefile) {
    HttpResponse response;
    CURL* curl;
    start_server("");
    curl = curl_easy_init();
    ASSERT_NE(curl, nullptr);

    // Configurações para o POST com arquivo
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/");
    curl_mime* mime;
    curl_mimepart* part;
    mime = curl_mime_init(curl);

    // Adiciona o arquivo ao POST
    part = curl_mime_addpart(mime);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, "Makefile");

    curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HeaderCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response);

    // Executa o POST
    CURLcode res = curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status_code);

    // Verifica o resultado
    EXPECT_EQ(res, CURLE_OK);
    EXPECT_EQ(response.status_code, 200);
    EXPECT_TRUE(response.body.find("Upload successful") != std::string::npos);

    // Limpeza
    curl_mime_free(mime);
    curl_easy_cleanup(curl);
    stop_server();
}
