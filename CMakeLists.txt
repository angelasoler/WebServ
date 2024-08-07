cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -Werror)

project(WebServ)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "../")

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
target_link_libraries(webservtest gtest_main)

include(GoogleTest)
gtest_discover_tests(webservtest)

add_custom_target(
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    COMMAND ${CMAKE_COMMAND} -E rm -rf ../webserv ../webservtest
)
