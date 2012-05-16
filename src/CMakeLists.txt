set(Explore_SRCS
    main.cpp
    game/Explore.cpp
    game/ExploreMenu.cpp
)

add_executable(Explore ${Explore_SRCS})

#find_package( Irrlicht REQUIRED )
set( IRRLICHT_LIBRARIES Irrlicht )
find_package( OpenAL REQUIRED )
find_package( Bullet COMPONENTS LinearMath BulletDynamics REQUIRED )

target_link_libraries( Explore
                        ${IRRLICHT_LIBRARIES}
                        ${OPENAL_LIBRARIES}
                        ${BULLET_LIBRARIES} )
