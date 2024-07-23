cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -Werror)

project(WebServ)

# Adicione as linhas para incluir e linkar o Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

include_directories(includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")
add_executable(webserv ${SOURCES})

list(REMOVE_ITEM SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
add_executable(webservtest test/webservtest.cpp ${SOURCES})
# target_link_libraries(webservtest gtest_main)
find_package(CURL REQUIRED)
target_link_libraries(webservtest PRIVATE gtest_main CURL::libcurl)

include(GoogleTest)
gtest_discover_tests(webservtest)
