cmake_minimum_required(VERSION 3.5)

set(NAME laldb-tests-bin)

execute_process(
	COMMAND ../../compile.sh --get-build-directory
	OUTPUT_VARIABLE OUTPUT_PATH)

project(${NAME})

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

include_directories(../includes)
include_directories(.)

set(SRCS
	main.cpp
	Datastore/tests.cpp
	Loader/tests.cpp)

link_directories(.)
add_executable(${NAME} ${SRCS})
target_link_libraries(${NAME} static_laldb)