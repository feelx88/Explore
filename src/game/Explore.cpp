#include "Explore.h"

#include <string>
#include <boost/property_tree/ini_parser.hpp>

#include <engine/EventReceiver.h>

using namespace irr;
using namespace core;

Explore::Explore()
    : mRunning( true ),
      mGameState( EGS_MAIN_MENU ),
      mMenu( 0 ),
      mConfig( new boost::property_tree::ptree )
{
    loadConfig();
    initIrrlicht();
    initMenu();
}

Explore::~Explore()
{
    mDevice->closeDevice();
}

int Explore::run()
{
    while( mRunning )
    {
        switch( mGameState )
        {
        case EGS_QUIT:
            mRunning = false;
            break;
        case EGS_MAIN_MENU:
            mGameState = mMenu->run();
            break;
        case EGS_GAME:
            break;
        }
    }
    return 0;
}

void Explore::loadConfig()
{
    try
    {
        boost::property_tree::ini_parser::read_ini( "config.ini", *mConfig );
    }
    catch( ... )
    {
        saveConfig();
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

    params.EventReceiver = new EventReceiver();

    mDevice = createDeviceEx( params );
    mVideoDriver = mDevice->getVideoDriver();
    mGUI = mDevice->getGUIEnvironment();

    saveConfig();
}

void Explore::initMenu()
{
    mMenu.reset( new ExploreMenu( mDevice, mConfig ) );
}
