cmake_minimum_required(VERSION 2.8)
project(pipes C CXX)

# Pipe library
add_library(pipelib
	src/pipe_repr.h 
	src/pipe.h
	src/pipe.c)

# Simple test
# Parent invoking, and communication with a child.
include_directories(src)
add_executable(parent tests/parent.cpp)
add_executable(child tests/child.cpp)
target_link_libraries(parent pipelib)
target_link_libraries(child pipelib)
add_custom_target(pipetest
	COMMAND $<TARGET_FILE:parent>
	        $<TARGET_FILE:child>
	DEPENDS parent child)

