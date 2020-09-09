#
# Set version
#
cmake_minimum_required(VERSION 3.0)

#
# Project Name
#
project(ma-test)

#
# Configuration macros
#
add_definitions(-DMEM_ALLOC_BUFFER_SIZE=1024)
add_definitions(-DMEM_ALLOC_UNITS_MAX=64)

#
# Paths
#
set(INC_PATH include)
set(SRC_PATH src)

#
# Source Files
#
set(C_SOURCES
    ${SRC_PATH}/mem_alloc.c
    ${SRC_PATH}/mem_alloc_test_main.c
    )

# Includes
include_directories(
    ${INC_PATH}
    )

# Executable
add_executable(test ${C_SOURCES})