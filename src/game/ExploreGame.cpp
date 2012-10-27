/*
    Copyright 2012 Felix Müller.

    This file is part of Explore.

    Explore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Explore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Explore.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "ExploreGame.h"
#include "Explore.h"
#include "players/WorldPlayer.h"
#include "players/LocalPlayer.h"
#include <engine/IrrlichtTools.h>
#include <engine/PathTools.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/Entity.h>
#include <engine/EntityContainer.h>
#include <engine/LuaScript.h>
#include <boost/property_tree/xml_parser.hpp>
#include "ItemFactory.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

LUABINDER_REGISTER_MODULE_START( ExploreGameBinder )
    class_<ExploreGame>( "ExploreGame" )
        .def( "setBulletDebugDraw", &ExploreGame::setBulletDebugDraw )
LUABINDER_REGISTER_MODULE_END( ExploreGameBinder )

ExploreGame::ExploreGame( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mSceneManager( mDevice->getSceneManager() ),
      mGUI( mDevice->getGUIEnvironment() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() ),
      mLua( explore->getLuaVM() ),
      mBulletDebugDraw( false )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    E_GAME_STATE state = EGS_GAME;

    mEventReceiver->lockMouse( true );

    mSceneManager->addLightSceneNode( 0, vector3df( 0.f, 50.f, 0.f ),
                                      SColorf( 1, 1, 1 ), 1000.f );

    EntityContainerPtr level( new EntityContainer(
                                  mDevice, mBulletWorld,
                                  PathTools::getAbsolutePath( "TestMap1" ) ) );

    vector3df spawnPos =
            level->getProperties()->get( "Spawn.Position", vector3df() );

    mWorldPlayer.reset( new WorldPlayer( mExplore ) );

    boost::shared_ptr<LocalPlayer> p( new LocalPlayer( mExplore, mWorldPlayer ) );
    p->getEntity()->setPosition( spawnPos );
    ItemFactory::create( mExplore, p, "SimpleForceGun.item" );
    ItemFactory::create( mExplore, p, "SimpleBlockSpawner.item" );
    ItemFactory::create( mExplore, p, "SimpleGun.item" );
    ItemFactory::create( mExplore, p, "SuzanneSpawner.item" );

    luabind::globals( mLua.get() )["Explore"]["WorldPlayer"] = (IPlayerPtr)mWorldPlayer;
    luabind::globals( mLua.get() )["Explore"]["LocalPlayer"] = (IPlayerPtr)p;
    luabind::globals( mLua.get() )["Explore"]["Game"] = this;

    mBulletWorld->setGravity( btVector3( 0.f, -10.f, 0.f ) );

    mExplore->getExploreServer()->setServerMode( true );

    btClock clock;

    while( running && mDevice->run() )
    {
        mBulletWorld->stepSimulation(
                    float( clock.getTimeMilliseconds() ) / 1000.f, 50 );
        clock.reset();

        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 0 ) );
        mSceneManager->drawAll();
        mGUI->drawAll();

        if( mEventReceiver->keyClicked( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            state = EGS_MAIN_MENU;
            running = false;
        }

        mWorldPlayer->update();

        if( mBulletDebugDraw )
            mBulletWorld->debugDrawWorld();

        mVideoDriver->endScene();

        mDevice->setWindowCaption( stringw( mVideoDriver->getFPS() ).c_str() );
        mExplore->getIOService()->poll();
    }

    mEventReceiver->lockMouse( false );
    luabind::globals( mLua.get() )["Explore"]["WorldPlayer"] = luabind::nil;
    luabind::globals( mLua.get() )["Explore"]["LocalPlayer"] = luabind::nil;
    luabind::globals( mLua.get() )["Explore"]["GameInstance"] = luabind::nil;

    mExplore->getExploreServer()->setServerMode( false );

    mWorldPlayer.reset();

    return state;
}

void ExploreGame::setBulletDebugDraw( bool enabled )
{
    mBulletDebugDraw = enabled;
}

WorldPlayerPtr ExploreGame::getWorldPlayer()
{
    return mWorldPlayer;
}
