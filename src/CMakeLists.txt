file( GLOB Engine_SRCS
    engine/*.cpp
    engine/*.h
    engine/*/*.cpp
    engine/*/*.h
)

file( GLOB Explore_SRCS
    main.cpp
    game/*.cpp
    game/*.h
    game/*/*.cpp
    game/*/*.h
)

set( ENGINE_LIBRARY_TYPE SHARED CACHE STRING
    "Build type of Engine library: STATIC or SHARED. Static builds work better with mingw." )

add_library( Engine ${ENGINE_LIBRARY_TYPE} ${Engine_SRCS} )

add_executable(Explore ${Explore_SRCS})

#Target for including asset files in qt creator tree
file( GLOB assets
    ../build/config.ini
    ../build/keycodes.ini
    ../build/log.txt
    ../build/init.lua
    ../build/data/*/*/*
    ../.gitignore
    ../README
)

add_custom_target( Assets SOURCES ${assets} )

#find_package( Irrlicht REQUIRED )
set( IRRLICHT_LIBRARIES Irrlicht )
#find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )
find_package( Lua51 REQUIRED )
find_package( Boost COMPONENTS system filesystem REQUIRED )
#find_package( luabind REQUIRED )
set( LUABIND_LIBRARIES luabind )

include_directories( ${BULLET_INCLUDE_DIR} )

target_link_libraries( Engine
                        ${IRRLICHT_LIBRARIES}
                        ${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES}
                        ${LUA_LIBRARIES}
                        ${Boost_LIBRARIES}
                        ${LUABIND_LIBRARIES} )
target_link_libraries( Explore Engine )

set_target_properties( Engine Explore PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ../build
    LIBRARY_OUTPUT_DIRECTORY ../build )
