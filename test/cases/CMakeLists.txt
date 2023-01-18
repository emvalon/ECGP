cmake_minimum_required(VERSION 3.8)
set(ROOT_PATH $ENV{ECGP_ROOT_PATH})

AUX_SOURCE_DIRECTORY(${ROOT_PATH}/util/testUtil/src TU_SRC)
AUX_SOURCE_DIRECTORY(./ CASE_SRC)

add_library(utcases 
    ${TU_SRC}
    ${CASE_SRC}
)