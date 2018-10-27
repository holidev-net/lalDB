cmake_minimum_required(VERSION 3.5)

set(NAME laldb-tests-bin)

execute_process(
	COMMAND ../../compile.sh --get-build-directory
	OUTPUT_VARIABLE OUTPUT_PATH)

project(${NAME})

set(CMAKE_C_STANDARD 11)

include_directories(../includes)
include_directories(.)

set(SRCS
	main.cpp
	Datastore/tests.cpp)

link_directories(.)
add_executable(${NAME} ${SRCS})
target_link_libraries(${NAME} static_laldb)