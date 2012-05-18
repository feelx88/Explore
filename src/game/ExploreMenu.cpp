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
    int windowWidth2 = mConfig->get<int>( "Engine.windowWidth", 640 ) / 2;
    int windowHeight5 = mConfig->get<int>( "Engine.windowHeight", 480 ) / 5;

    irr::gui::IGUIButton *buttonNewGame = mGUI->addButton(
                recti( windowWidth2 - 50, 1 * windowHeight5 - 10,
                       windowWidth2 + 50, 1 * windowHeight5 + 10 ), 0, -1, L"New Game" );

    irr::gui::IGUIButton *buttonLoadGame = mGUI->addButton(
                recti( windowWidth2 - 50, 2 * windowHeight5 - 10,
                       windowWidth2 + 50, 2 * windowHeight5 + 10 ), 0, -1, L"Load Game" );

    irr::gui::IGUIButton *buttonOptions = mGUI->addButton(
                recti( windowWidth2 - 50, 3 * windowHeight5 - 10,
                       windowWidth2 + 50, 3 * windowHeight5 + 10 ), 0, -1, L"Options" );

    irr::gui::IGUIButton *buttonQuit = mGUI->addButton(
                recti( windowWidth2 - 50, 4 * windowHeight5 - 10,
                       windowWidth2 + 50, 4 * windowHeight5 + 10 ), 0, -1, L"Quit" );

    E_GAME_STATE state = EGS_QUIT;
    bool running = true;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene();
        mGUI->drawAll();

        if( buttonNewGame->isPressed() )
            _LOG( "New Game button pressed" );
        if( buttonLoadGame->isPressed() )
            _LOG( "Load Game button pressed" );
        if( buttonOptions->isPressed() )
            _LOG( "Options button pressed" );

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
