#include "Explore.h"

#include <string>
#include <boost/property_tree/ini_parser.hpp>

#include <engine/EventReceiver.h>

using namespace irr;
using namespace core;

Explore::Explore()
    : mRunning( true ),
      mGameState( EGS_MAIN_MENU ),
      mMenu( 0 )
{
    loadConfig();
    initIrrlicht();
    initMenu();
}

int Explore::run()
{
    while( mRunning && mDevice->run() )
    {
        mVideoDriver->beginScene();

        switch( mGameState )
        {
        case EGS_QUIT:
            mRunning = false;
            break;
        case EGS_MAIN_MENU:
            mMenu->run();
            break;
        case EGS_GAME:
            break;
        }

        mVideoDriver->endScene();
    }
    return 0;
}

void Explore::setGameState( Explore::E_GAME_STATE state )
{
    mGameState = state;
}

void Explore::loadConfig()
{
    try
    {
        boost::property_tree::ini_parser::read_ini( "config.ini", mConfig );
    }
    catch( ... )
    {
        saveConfig();
    }
}

void Explore::saveConfig()
{
    boost::property_tree::ini_parser::write_ini( "config.ini", mConfig );
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

    params.EventReceiver = new EventReceiver();

    mDevice.reset( createDeviceEx( params ) );
    mVideoDriver = mDevice->getVideoDriver();

    saveConfig();
}

void Explore::initMenu()
{
    mMenu.reset( new ExploreMenu( mDevice ) );
}
