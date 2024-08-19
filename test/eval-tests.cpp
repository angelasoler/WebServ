#include <gtest/gtest.h>
#include "tests.hpp"

// - verificar uso explicitamente os restornos de recv e send, 0 ou -1
// - remove client if theres an error on recv or send

// - cgi deve retornar corretamente caso o script reronar erro ou for um loop infinito

// - tratar todos os http status
// - settar multiplos servidor com multiplos hostnames
// - definir metodos aceitos para certa rota
        // -tentar usar DELETE com e sem permissão
        // - POST e GET com e sem permissão
// - se conectar ao server usando o browser, atraves de network

// - Try a wrong URL on the server.
// - Try to list a directory.
// - Try a redirected URL.


// - Use Siege to run some stress tests.
// - Availability should be above 99.5% for a simple GET
        // on an empty page with a siege -b on that page.
// - You should be able to use siege indefinitely
        // without having to restart the server (take a look at siege -b).