#include <gtest/gtest.h>
#include "tests.hpp"

// [x] verificar uso explicitamente os restornos de recv e send, 0 ou -1
// [x] remove client if theres an error on recv or send

// [_] testar com siege

// config:
    // [_] status code most be accurate

    // [_] Multiples server with multiples ports:
    // curl --resolve example.com:80:127.0.0.1 http://example.com/

    // [/] use default error page

    // [/] limit the client body

    // [x] Setup routes in a server to different directories.

    // [x] Setup a default file to search for if you ask for a directory.

    // [/] Setup a list of methods accepted for a certain route
    //     (e.g., try to delete something with and without permission).

// Basic checks
    // [_]GET, POST and DELETE requests should work.
    // [_]UNKNOWN requests should not result in a crash.
    // [_]For every test you should receive the appropriate status code.
    // [_] Upload some file to the server and get it back.

// CGI
    // [_] The CGI should be run in the correct directory for relative path
    //     file access.
    // [_] With the help of the students you should check that everything is
    //     working properly. You have to test the CGI with the "GET" and "POST" methods.
    // [_] You need to test with files containing errors to see if the error
    //     handling works properly. You can use a script containing an infinite
    //     loop or an error; you are free to do whatever tests you want within the
    //     limits of acceptability that remain at your discretion. The group being
    //     evaluated should help you with this. 
    // [_] The server should never crash
    // [_] An error should be visible in case of a problem.

// browser
    // [_]Use the reference browser of the team. Open the network part of it, and try to
    //    connect to the server using it.
    // [_]Look at the request header and response header.
    // [_]It should be compatible to serve a fully static website.
    // [_]Try a wrong URL on the server.
    // [_]Try to list a directory.
    // [_]Try a redirected URL.
    // [_]Try anything you want to.

// Port issues
    // [_]In the configuration file setup multiple ports and use different
    //    websites. Use the browser to ensure that the configuration works
    //    as expected and shows the right website.

// Siege & stress test
    // [_] Use Siege to run some stress tests.
    // [_] Availability should be above 99.5% for a simple GET on an empty page
    //     with a siege -b on that page.
    // [_] Verify there is no memory leak (Monitor the process memory usage.
    //     It should not go up indefinitely).
    // [_] Check if there is no hanging connection.
    // [_] You should be able to use siege indefinitely without having to restart
    //     the server (take a look at siege -b).

