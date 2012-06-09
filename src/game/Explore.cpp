/*
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


#include "Explore.h"

#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>

#include <engine/IrrlichtTools.h>
#include <engine/PathTools.h>
#include <engine/LuaTools.h>
#include <engine/EventReceiver.h>

#include "ExploreMenu.h"
#include "ExploreGame.h"

using namespace irr;
using namespace core;

Explore::Explore()
    : mConfig( new boost::property_tree::ptree ),
      mLogFile( new std::ofstream( "log.txt" ) ),
      mEventReceiver( new EventReceiver() ),
      mRunning( true ),
      mGameState( EGS_MAIN_MENU ),
      mMenu( 0 )
{
    LoggerSingleton::instance().addStream( *mLogFile );
    loadConfig();
    initIrrlicht();
    initBullet();
    initLua();
    initMenu();
    initGame();
}

Explore::~Explore()
{
}

int Explore::run()
{
    while( mRunning )
    {
        switch( mGameState )
        {
        case EGS_QUIT:
            _LOG( "Quit application" );
            mRunning = false;
            break;
        case EGS_MAIN_MENU:
            mGameState = mMenu->run();
            break;
        case EGS_GAME:
            mGameState = mGame->run();
            break;
        }
    }
    return 0;
}

void Explore::setGameState( const E_GAME_STATE &state )
{
    mGameState = state;
}

IrrlichtDevicePtr Explore::getIrrlichtDevice() const
{
    return mDevice;
}

EventReceiverPtr Explore::getEventReceiver() const
{
    return mEventReceiver;
}

BulletWorldPtr Explore::getBulletWorld() const
{
    return mBulletWorld;
}

LuaStatePtr Explore::getLuaVM() const
{
    return mLua;
}

void Explore::loadConfig()
{
    try
    {
        boost::property_tree::ini_parser::read_ini( "config.ini", *mConfig );
    }
    catch( ... )
    {
        _LOG( "No config file found, creating default one." );
        saveConfig();
    }

    boost::optional<boost::property_tree::ptree&> paths =
            mConfig->get_child_optional( "Paths" );

    if( paths )
    {
        for( boost::property_tree::ptree::iterator x = ( *paths ).begin();
             x != ( *paths ).end(); ++x )
        {
            PathTools::addPath( x->second.data() );
        }
    }
}

void Explore::saveConfig()
{
    boost::property_tree::ini_parser::write_ini( "config.ini", *mConfig );
}

void Explore::initIrrlicht()
{
    SIrrlichtCreationParameters params;
    std::string deviceType =
            readConfigValue<std::string>( "Engine.deviceType", "Software" );

    if( deviceType == "OpenGL" )
        params.DriverType = video::EDT_OPENGL;
    else
        params.DriverType = video::EDT_BURNINGSVIDEO;

    params.WindowSize = dimension2di(
                readConfigValue<int>( "Engine.windowWidth", 640 ),
                readConfigValue<int>( "Engine.windowHeight", 480 ) );
    params.Bits = readConfigValue<int>( "Engine.colorDepth", 16 );
    params.Fullscreen = readConfigValue<bool>( "Engine.fullscreen", false );
    params.Vsync = readConfigValue<bool>( "Engine.verticalSync", false );
    params.AntiAlias = readConfigValue<int>( "Engine.antiAliasing", 0 );
    params.Vsync = readConfigValue<bool>( "Engine.verticalSync", false );

    params.EventReceiver = mEventReceiver.get();

    mDevice = IrrlichtTools::createIrrlichtDevicePtr( createDeviceEx( params ) );
    mVideoDriver = mDevice->getVideoDriver();
    mGUI = mDevice->getGUIEnvironment();

    mEventReceiver->setDevice( mDevice );

    mVideoDriver->getMaterial2D().ZBuffer = false;

    irr::gui::IGUISkin *skin = mGUI->getSkin();

    skin->setFont( mGUI->getFont(
                       readConfigValue<std::string>( "Engine.fontFile", "data/bitstream_vera_sans_12.xml" ).c_str() ) );
    mGUI->setSkin( skin );

    mDevice->getSceneManager()->getParameters()->setAttribute(
                irr::scene::OBJ_TEXTURE_PATH,
                PathTools::getAbsolutePath(
                    mConfig->get<std::string>( "Paths.TexturePath", "" ) ).c_str() );

    saveConfig();
}

void Explore::initBullet()
{
    mCollisionConfiguration.reset( new btDefaultCollisionConfiguration() );

    ///use the default collision dispatcher. For parallel processing you can
    // use a diffent dispatcher (see Extras/BulletMultiThreaded)^
    mDispatcher.reset( new	btCollisionDispatcher( mCollisionConfiguration.get() ) );

    ///btDbvtBroadphase is a good general purpose broadphase. You
    // can also try out btAxis3Sweep.
    mBroadphase.reset( new btDbvtBroadphase() );

    ///the default constraint solver. For parallel processing
    // you can use a different solver (see Extras/BulletMultiThreaded)
    mConstraintSolver.reset( new btSequentialImpulseConstraintSolver() );

    mBulletWorld.reset( new btDiscreteDynamicsWorld(
                            mDispatcher.get(),
                            mBroadphase.get(),
                            mConstraintSolver.get(),
                            mCollisionConfiguration.get() ) );

    mBulletDebugDrawer.reset( new BulletIrrlichtDebugDrawer() );
    mBulletDebugDrawer->driver = mDevice->getVideoDriver();
    mBulletWorld->setDebugDrawer( mBulletDebugDrawer.get() );
}

void Explore::initLua()
{
    mLua = LuaTools::createLuaVM();
}

void Explore::initMenu()
{
    mMenu.reset( new ExploreMenu( this ) );
}

void Explore::initGame()
{
    mGame.reset( new ExploreGame( this ) );
}
