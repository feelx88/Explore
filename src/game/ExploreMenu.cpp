#include "ExploreMenu.h"

using namespace irr;
using namespace core;

ExploreMenu::ExploreMenu( IrrlichtDevicePtr device )
    : mDevice( device ),
      mGUI( mDevice->getGUIEnvironment() ),
      mVideoDriver( mDevice->getVideoDriver() )
{
}

E_GAME_STATE ExploreMenu::run()
{
    mButtonQuit = mGUI->addButton( recti( 0, 0, 100, 20 ), 0, -1, L"Quit" );

    E_GAME_STATE state = EGS_QUIT;
    bool running = true;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene();
        mGUI->drawAll();

        if( mButtonQuit->isPressed() )
        {
            state = EGS_QUIT;
            running = false;
        }

        mVideoDriver->endScene();
    }

    mGUI->clear();
    return state;
}
