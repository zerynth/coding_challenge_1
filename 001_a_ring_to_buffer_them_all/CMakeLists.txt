#
# Set version
#
cmake_minimum_required(VERSION 3.0)

#
# Project Name
#
project(rb-test)

#
# Configuration macros
#
add_definitions(-DRING_BUFFER_USED_FROM_ISR=0)
add_definitions(-DRING_BUFFER_USED_FROM_THREAD=0)

#
# Paths
#
set(INC_PATH include)
set(SRC_PATH src)

#
# Source Files
#
set(C_SOURCES
    ${SRC_PATH}/ring_buffer.c
    ${SRC_PATH}/ring_buffer_test_main.c
    )

# Includes
include_directories(
    ${INC_PATH}
    )

# Executable
add_executable(rb-test ${C_SOURCES})