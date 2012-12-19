if( NOT WIN32 )
	set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic" )
endif()

project(Explore)
cmake_minimum_required(VERSION 2.8)

include_directories( src )

add_subdirectory(src)

