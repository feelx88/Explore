file( GLOB Engine_SRCS
    engine/*.cpp
    ../include/engine/*.h
    engine/*/*.cpp
    ../include/engine/*/*.h
)

file( GLOB Explore_SRCS
    game/*.cpp
    ../include/game/*.h
    game/*/*.cpp
    ../include/game/*/*.h
)

file( GLOB EngineBindings_SRC
    engineBindings/*
)

file( GLOB GameBindings_SRC
    gameBindings/*
)

set( ENGINE_LIBRARY_TYPE SHARED CACHE STRING
    "Build type of Engine library: STATIC or SHARED. Static builds work better with mingw." )

add_library( Engine ${ENGINE_LIBRARY_TYPE} ${Engine_SRCS} )
add_library( ExploreBind SHARED ${EngineBindings_SRC} ${GameBindings_SRC} )
add_library( Explore ${ENGINE_LIBRARY_TYPE} ${Explore_SRCS} )

add_executable( Explore.bin main.cpp )

#Target for including asset files in qt creator tree
file( GLOB_RECURSE assets
    ../build/config.ini
    ../build/keycodes.ini
    ../build/log.txt
    ../build/init.py
    ../build/data/*
    ../.gitignore
    ../README
)

add_custom_target( Assets SOURCES ${assets} )

#find_package( Irrlicht REQUIRED )
set( IRRLICHT_LIBRARIES Irrlicht CACHE FILEPATH "Irrlicht library" )
set( IRRLICHT_INCLUDE_DIR CACHE PATH "Irrlicht Include directory" )
#find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )
find_package( PythonLibs REQUIRED )
set( BOOST_PYTHON_VERSION "python3" CACHE STRING "Library version for boost_python; eiter python or python3" )
find_package( Boost COMPONENTS system filesystem ${BOOST_PYTHON_VERSION} REQUIRED )
set( boost_libs ${boost_libs} ${Boost_LIBRARIES} )
find_package( Boost COMPONENTS chrono )
set( boost_libs ${boost_libs} ${Boost_LIBRARIES} )
find_package( Boost COMPONENTS foreach )
set( boost_libs ${boost_libs} ${Boost_LIBRARIES} )

include_directories( ${BULLET_INCLUDE_DIR}
                       ${PYTHON_INCLUDE_DIR}
                        ${Boost_INCLUDE_DIR}
                        ${IRRLICHT_INCLUDE_DIR} )

target_link_libraries( Engine
                        ${IRRLICHT_LIBRARIES}
                        #${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES}
                        ${PYTHON_LIBRARY}
                        ${boost_libs} )

target_link_libraries( Explore Engine )
target_link_libraries( ExploreBind Explore )
target_link_libraries( Explore.bin ExploreBind )

set_target_properties( Engine Explore ExploreBind Explore.bin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ../build
    LIBRARY_OUTPUT_DIRECTORY ../build )

set_target_properties( ExploreBind PROPERTIES
    PREFIX "" )

if( WIN32 )
    set_target_properties( Engine PROPERTIES COMPILE_FLAGS -DENGINE_EXPORTS )
    set_target_properties( Explore PROPERTIES COMPILE_FLAGS -DENGINE_EXPORTS )
    set_target_properties( ExploreBind PROPERTIES SUFFIX .pyd )
endif()
