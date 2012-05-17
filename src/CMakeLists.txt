file(GLOB Explore_SRCS
    *.cpp
    *.h
    */*.cpp
    */*.h
)

add_executable(Explore ${Explore_SRCS})

#Target for including asset files in qt creator tree
file( GLOB assets
    config.ini
)

add_custom_target( Assets SOURCES ${assets} )

#find_package( Irrlicht REQUIRED )
set( IRRLICHT_LIBRARIES Irrlicht )
find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )

target_link_libraries( Explore
                        ${IRRLICHT_LIBRARIES}
                        ${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES} )
