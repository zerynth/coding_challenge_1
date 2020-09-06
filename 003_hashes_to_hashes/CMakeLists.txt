#
# Set version
#
cmake_minimum_required(VERSION 3.0)

#
# Project Name
#
project(ht-test)

#
# Configuration macros
#
add_definitions(-DHT_DEBUG=1)
add_definitions(-DHT_NODES_MAX=254)
add_definitions(-DHT_VALUE_SIZE_MAX=32)
add_definitions(-DHT_TRIM_VALUE_LEN=0)

#
# Paths
#
set(INC_PATH include)
set(SRC_PATH src)

#
# Source Files
#
set(C_SOURCES
    ${SRC_PATH}/hash_table.c
    ${SRC_PATH}/hash_table_test_main.c
    )

# Includes
include_directories(
    ${INC_PATH}
    )

# Executable
add_executable(test ${C_SOURCES})