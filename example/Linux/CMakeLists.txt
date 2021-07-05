cmake_minimum_required(VERSION 2.8.5)

set(ROOT_PAT ${PROJECT_SOURCE_DIR}/../..)

include(${ROOT_PAT}/proj.cmake)

add_executable(${PROJECT_NAME} ${SRC}
    "main.c"        
)
