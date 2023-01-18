cmake_minimum_required(VERSION 3.8)
set(ROOT_PATH $ENV{ECGP_ROOT_PATH})

project(example C)
include(${ROOT_PATH}/proj.cmake)

set(PORT ${ROOT_PATH}/src/port/os/Linux/os_port.c)

set(CMAKE_C_FLAGS "-O0 -ggdb")

add_executable(${PROJECT_NAME} 
    ${SRC}
    ${PORT}
    "main.c"
)