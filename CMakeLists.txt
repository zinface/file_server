cmake_minimum_required(VERSION 3.5.1)

project(file-server VERSION 0.1)

include(cmake/BuildSourceTgz.cmake)

add_executable(server server.c wrap.c common.c)
add_executable(client client.c wrap.c common.c)
add_executable(progress progress.c)


# add_executable(echo_server echo_server.c wrap.c)
# add_executable(echo_client echo_client.c wrap.c)


# if you want build this project
# please run: 
#   mkdir build && cd build && cmake .. && make