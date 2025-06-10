cmake_minimum_required(VERSION 3.14)
project(GatewayEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(APPLE)
    include_directories(include /opt/homebrew/include)
elseif(UNIX)
    include_directories(include /usr/include)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(gateway_server ${SOURCES})

target_compile_options(gateway_server PRIVATE -O3 -Wall -Wextra)