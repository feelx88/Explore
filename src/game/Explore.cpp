#include "Explore.h"

#include <string>
#include <boost/property_tree/ini_parser.hpp>

#include <engine/EventReceiver.h>

using namespace irr;
using namespace core;

Explore::Explore()
    : mRunning( true ),
      mMenu( 0 )
{
    loadConfig();
    initIrrlicht();
}

int Explore::run()
{
    while( mRunning && mDevice->run() )
    {
    }
    return 0;
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
            mConfig.get<std::string>( "Engine.deviceType", "Software" );

    if( deviceType == "OpenGL" )
        params.DriverType = video::EDT_OPENGL;
    else
        params.DriverType = video::EDT_BURNINGSVIDEO;

    params.WindowSize = dimension2di(
                mConfig.get<int>( "Engine.windowWidth", 640 ),
                mConfig.get<int>( "Engine.windowHeight", 480 ) );
    params.Bits = mConfig.get<int>( "Engine.colorDepth", 16 );
    params.Fullscreen = mConfig.get<bool>( "Engine.fullscreen", false );
    params.Vsync = mConfig.get<bool>( "Engine.verticalSync", false );

    params.EventReceiver = new EventReceiver();

    mDevice.reset( createDeviceEx( params ) );

    saveConfig();
}
