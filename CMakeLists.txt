if( NOT WIN32 )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Wno-long-long" )
elseif( NOT MINGW )
        set( BOOST_ROOT "c:/boost/" CACHE PATH
                "Root directory of boost installation, has to be set on windows" )
        set( Boost_COMPILER "-vc100" CACHE STRING
                "Compiler extension for boost, e.g. MSVC 2010 uses -vc100" )
        set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DBOOST_ALL_NO_LIB /DBOOST_PYTHON_STATIC_LIB /D_WIN32_WINNT=0x0501" )
endif()

project(Explore)
cmake_minimum_required(VERSION 2.8)

include_directories( include include/engine include/game )

add_subdirectory( src )
