file(GLOB Explore_SRCS
    *.cpp
    *.h
    */*.cpp
    */*.h
    */*/*.cpp
    */*/*.h
)

add_executable(Explore ${Explore_SRCS})

#Target for including asset files in qt creator tree
file( GLOB assets
    ../build/config.ini
    ../build/keycodes.ini
    ../build/log.txt
    ../build/data/*/*/*
    ../.gitignore
    ../README
)

add_custom_target( Assets SOURCES ${assets} )

#find_package( Irrlicht REQUIRED )
set( IRRLICHT_LIBRARIES Irrlicht )
find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )
find_package( Lua51 REQUIRED )
find_package( Boost COMPONENTS system filesystem REQUIRED )

include_directories( ${BULLET_INCLUDE_DIR} )

target_link_libraries( Explore
                        ${IRRLICHT_LIBRARIES}
                        ${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES}
                        ${LUA_LIBRARIES}
                        ${Boost_LIBRARIES} )

set_target_properties( Explore PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../build )
