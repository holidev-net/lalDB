cmake_minimum_required(VERSION 3.5)

set(SHARED_NAME laldb)
set(STATIC_NAME static_${SHARED_NAME})
set(VERSION 0.0.1)

string(REGEX MATCH "([0-9]*).([0-9]*).([0-9]*)" _ ${VERSION})
set(MAJOR ${CMAKE_MATCH_1})
set(MINOR ${CMAKE_MATCH_2})
set(PATCH ${CMAKE_MATCH_3})

project(${SHARED_NAME} VERSION ${VERSION} DESCRIPTION "Lite and Lazy Database -- shared build")
project(${STATIC_NAME} VERSION ${VERSION} DESCRIPTION "Lite and Lazy Database -- static build")

set(CMAKE_C_STANDARD 11)

include_directories(includes)
include_directories(.)

set(SRCS
	srcs/Loader/Loader.cpp
	srcs/Datastore/Datastore.cpp)

set(PUBLIC_HEADERS
	includes/lalbd.hpp
	includes/Loader/Loader.hpp
	includes/Datastore/Datastore.hpp
	includes/DataRepresentation/DataRepresentation.hpp
	inclides/DataRepresentation/DataRepresentation.tpp)


add_library(${SHARED_NAME} SHARED ${SRCS})
add_library(${STATIC_NAME} STATIC ${SRCS})

set_target_properties(${SHARED_NAME} PROPERTIES SOVERSION ${MAJOR})
set_target_properties(${STATIC_NAME} PROPERTIES SOVERSION ${MAJOR})

set_target_properties(${SHARED_NAME} PROPERTIES VERSION ${PROJECT_VERSION})
set_target_properties(${STATIC_NAME} PROPERTIES VERSION ${PROJECT_VERSION})

set_target_properties(${SHARED_NAME} PROPERTIES PUBLIC_HEADER ${PUBLIC_HEADERS})
set_target_properties(${STATIC_NAME} PROPERTIES PUBLIC_HEADER ${PUBLIC_HEADERS})

set_target_properties(${SHARED_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/shared)
set_target_properties(${STATIC_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin/static)
	