#include "ExploreMenu.h"
#include <engine/LoggerSingleton.h>

using namespace irr;
using namespace core;

ExploreMenu::ExploreMenu( IrrlichtDevicePtr device, PropTreePtr config )
    : mDevice( device ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mGUI( mDevice->getGUIEnvironment() ),
      mConfig( config )
{
}

E_GAME_STATE ExploreMenu::run()
{
    int windowWidth = mConfig->get<int>( "Engine.windowWidth", 640 );
    int windowHeight = mConfig->get<int>( "Engine.windowHeight", 480 );

    irr::gui::IGUIButton *buttonQuit = mGUI->addButton(
                recti( windowWidth / 2 - 50, windowHeight / 2 - 10,
                       windowWidth / 2 + 50, windowHeight / 2 + 10 ), 0, -1, L"Quit" );

    E_GAME_STATE state = EGS_QUIT;
    bool running = true;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene();
        mGUI->drawAll();

        if( buttonQuit->isPressed() )
        {
            _LOG( "Quit button pressed" );
            state = EGS_QUIT;
            running = false;
        }

        mVideoDriver->endScene();
    }

    mGUI->clear();
    return state;
}
