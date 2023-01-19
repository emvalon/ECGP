cmake_minimum_required(VERSION 3.8)
set(ROOT_PATH $ENV{ECGP_ROOT_PATH})

project(UnitTest C)
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")

include(${ROOT_PATH}/CMakeLists.txt)
include_directories(${ROOT_PATH}/util/testUtil/include())
add_subdirectory("cases")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --coverage")

set(PORT ${ROOT_PATH}/src/port/os/Linux/os_port.c)

add_executable(${PROJECT_NAME}
    ${ECGP_SRC}
    ${PORT}
    "main.c"
)
target_link_libraries(${PROJECT_NAME} utcases)