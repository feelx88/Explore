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


#include "Explore.h"

#include <string>
#include <boost/property_tree/ini_parser.hpp>
#include <fstream>

#include <engine/IrrlichtTools.h>
#include <engine/PathTools.h>
#include <engine/LuaTools.h>
#include <engine/LuaBinder.h>
#include <engine/EventReceiver.h>

#include "ExploreMenu.h"
#include "ExploreGame.h"

#include "ItemCache.h"

using namespace irr;
using namespace core;

PropTreePtr Explore::sKeyCodes( new boost::property_tree::ptree() );

class ExploreBinder : public LuaBinder
{
public:
    void reg( LuaStatePtr state )
    {
        using namespace luabind;
        module( state.get() )
        [
                def( "exit", &exit ),
                class_<Explore>( "Explore" )
                    .def( "readConfigValue", &Explore::readConfigValue<std::string> )
                    .def( "getExploreServer", &Explore::getExploreServer )
                    .scope
                    [
                        def( "getKeyCode", &Explore::getKeyCode )
                    ]
        ];
    }

private:
    static int regDummy;
};
int ExploreBinder::regDummy = LuaBinder::registerBinder( new ExploreBinder );

struct ScriptConsoleKeyCallback : public EventReceiver::KeyCallback
{
    ScriptConsoleKeyCallback( ScriptConsolePtr console )
        : mPressed( false ),
          mConsole( console )
    {}

    bool call( SEvent::SKeyInput evt )
    {
        if( evt.PressedDown && !mPressed )
        {
            mConsole->setVisible( !mConsole->visible() );
            mPressed = true;
        }
        else if( !evt.PressedDown && mPressed )
            mPressed = false;
        return true;
    }
    bool mPressed;
    ScriptConsolePtr mConsole;
};

Explore::Explore()
    : mConfig( new boost::property_tree::ptree ),
      mLogFile( new std::ofstream( "log.txt" ) ),
      mEventReceiver( new EventReceiver() ),
      mRunning( true ),
      mGameState( EGS_MAIN_MENU )
{
    LoggerSingleton::instance().addStream( *mLogFile );
    mIOService.reset( new boost::asio::io_service() );

    loadConfig();
    initIrrlicht();
    initBullet();
    initLua();
    initScriptConsole();
    initMenu();
    initGame();
    initServer();
}

Explore::~Explore()
{
    saveConfig();
}

int Explore::run()
{
    while( mRunning && mDevice->run() )
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
        case EGS_CONNECT:
        case EGS_LOAD:
        default:
            break;
        }
        mIOService->poll();
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

ScriptConsolePtr Explore::getScriptConsole() const
{
    return mScriptConsole;
}

IOServicePtr Explore::getIOService() const
{
    return mIOService;
}

ExploreServerPtr Explore::getExploreServer() const
{
    return mServer;
}

const StringVector &Explore::getAvailableItems() const
{
    return mAvailableItems;
}

EKEY_CODE Explore::getKeyCode( const std::string &name )
{
    return static_cast<EKEY_CODE>( sKeyCodes->get( name, 0 ) );
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

    boost::filesystem::directory_iterator items(
                readConfigValue<std::string>( "Paths.ItemPath", "data/Items" ) ),
            end;

    for( ; items != end; ++items )
    {
        boost::filesystem::path p( items->path() );
        if( p.filename().extension() == ".item" )
        {
            mAvailableItems.push_back( p.filename().string() );
            _LOG( "Item found", p.filename().string() );
        }
    }

    //Set default Paths
    readConfigValue<std::string>( "Paths.DataPath", "data" );
    readConfigValue<std::string>( "Paths.MapPath", "data/Maps" );
    readConfigValue<std::string>( "Paths.EntityPath", "data/Entities" );

}

void Explore::saveConfig()
{
    boost::property_tree::ini_parser::write_ini( "config.ini", *mConfig );
}

void Explore::initIrrlicht()
{
    SIrrlichtCreationParameters params;
    std::string deviceType =
            readConfigValue<std::string>( "Engine.DeviceType", "OpenGL" );

    if( deviceType == "OpenGL" )
        params.DriverType = video::EDT_OPENGL;
    else
        params.DriverType = video::EDT_BURNINGSVIDEO;

    params.WindowSize = dimension2di(
                readConfigValue<int>( "Engine.WindowWidth", 640 ),
                readConfigValue<int>( "Engine.WindowHeight", 480 ) );
    params.Bits = readConfigValue<int>( "Engine.ColorDepth", 16 );
    params.Fullscreen = readConfigValue<bool>( "Engine.Fullscreen", false );
    params.Vsync = readConfigValue<bool>( "Engine.VerticalSync", false );
    params.AntiAlias = readConfigValue<int>( "Engine.AntiAliasing", 0 );
    params.Vsync = readConfigValue<bool>( "Engine.VerticalSync", false );

    params.EventReceiver = mEventReceiver.get();

    mDevice = IrrlichtTools::createIrrlichtDevicePtr( createDeviceEx( params ) );
    mVideoDriver = mDevice->getVideoDriver();
    mGUI = mDevice->getGUIEnvironment();

    mEventReceiver->setDevice( mDevice );

    mVideoDriver->getMaterial2D().ZBuffer = false;

    irr::gui::IGUISkin *skin = mGUI->getSkin();

    skin->setFont( mGUI->getFont(
                       readConfigValue<std::string>( "Engine.FontFile", "data/bitstream_vera_sans_12.xml" ).c_str() ) );
    mGUI->setSkin( skin );

    mDevice->getSceneManager()->getParameters()->setAttribute(
                irr::scene::OBJ_TEXTURE_PATH,
                PathTools::getAbsolutePath(
                    readConfigValue<std::string>( "Paths.TexturePath", "data/Textures" ) ).c_str() );

    boost::property_tree::ini_parser::read_ini( "keycodes.ini", *sKeyCodes );

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
    luabind::open( mLua.get() );
    LuaBinder::registerAll( mLua );

    luabind::globals( mLua.get() )["Explore"]["Instance"] = this;

    LuaTools::execFile( mLua, "init.lua" );
}

void Explore::initScriptConsole()
{
    mScriptConsole.reset( new ScriptConsole( mDevice, mLua, mConfig ) );
    mEventReceiver->setScriptConsole( mScriptConsole );
    mScriptConsole->setVisible( false );

    //Register callback
    ScriptConsoleKeyCallback *callback =
            new ScriptConsoleKeyCallback( mScriptConsole );
    mEventReceiver->registerKeyCallback( callback, KEY_F12 );
}

void Explore::initMenu()
{
    mMenu.reset( new ExploreMenu( this ) );
}

void Explore::initGame()
{
    mGame.reset( new ExploreGame( this ) );

    ItemCachePtr cache = ItemCache::instance();

    for( StringVector::iterator x = mAvailableItems.begin();
         x != mAvailableItems.end(); ++x )
    {
        cache->addItem( *x );
    }
}

void Explore::initServer()
{
    ExploreServer::HostInfo info;
    info.hostName = readConfigValue<std::string>( "Server.Name", "ExploreServer" );
    info.serverMaxPlayers = readConfigValue<int>( "Server.MaxPlayers", 8 );
    info.serverConnectedPlayers = 0;
    info.passwordHash = readConfigValue<std::string>( "Server.PasswordHash", "" );

    mMessenger.reset( new NetworkMessenger( mIOService, mConfig ) );
    mServer.reset( new ExploreServer( info, mMessenger ) );
}
