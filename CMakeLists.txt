#cmake_minimum_required(VERSION 3.2)
cmake_minimum_required(VERSION 2.8)
project(pipes C CXX)

# Pipe library
add_library(pipelib
	src/pipe_repr.h 
	src/pipe.h
	src/pipe.c)

# Simple test
include_directories(src)
add_executable(parent tests/parent.cpp)
target_link_libraries(parent pipelib)

include_directories(src)
add_executable(child tests/child.cpp)
target_link_libraries(child pipelib)

add_custom_target(pipetest
	COMMAND $<TARGET_FILE:parent>
	        $<TARGET_FILE:child>
	DEPENDS parent child)

## Testing dependencies
#add_subdirectory(dep/googletest)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})
#
## Tests
#include_directories(src)
#add_executable(pipe_test tests/pipe_test.cpp)
#target_link_libraries(pipe_test pipelib gtest gtest_main)
#
#add_custom_target(test
#	COMMAND pipe_test
#	DEPENDS pipe_test)
