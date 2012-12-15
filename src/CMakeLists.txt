file( GLOB Engine_SRCS
    engine/*.cpp
    engine/*.h
    engine/*/*.cpp
    engine/*/*.h
)

file( GLOB Explore_SRCS
    game/*.cpp
    game/*.h
    game/*/*.cpp
    game/*/*.h
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
set( IRRLICHT_LIBRARIES Irrlicht )
#find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )
find_package( PythonLibs REQUIRED )
find_package( Boost COMPONENTS system filesystem python REQUIRED )
find_package( Boost COMPONENTS chrono )
find_package( Boost COMPONENTS foreach )

include_directories( ${BULLET_INCLUDE_DIR}
                       ${PYTHON_INCLUDE_DIR} )

target_link_libraries( Engine
                        ${IRRLICHT_LIBRARIES}
                        #${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES}
                        ${PYTHON_LIBRARY}
                        ${Boost_LIBRARIES} )
target_link_libraries( Explore Engine )
target_link_libraries( ExploreBind Explore )
target_link_libraries( Explore.bin ExploreBind )

set_target_properties( Engine Explore ExploreBind Explore.bin PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ../build
    LIBRARY_OUTPUT_DIRECTORY ../build )

set_target_properties( ExploreBind PROPERTIES
    PREFIX "" )
