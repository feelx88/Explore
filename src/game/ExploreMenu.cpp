#include "ExploreMenu.h"

using namespace irr;
using namespace core;

ExploreMenu::ExploreMenu( IrrlichtDevicePtr device, PropTreePtr config )
    : mDevice( device ),
      mGUI( mDevice->getGUIEnvironment() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mConfig( config )
{
}

E_GAME_STATE ExploreMenu::run()
{
    int windowWidth = mConfig->get<int>( "Engine.windowWidth", 640 );
    int windowHeight = mConfig->get<int>( "Engine.windowHeight", 480 );

    irr::gui::IGUIButton *buttonQuit = mGUI->addButton(
                recti( position2di( windowWidth / 2, windowHeight / 2 ),
                       dimension2di( 100, 20 ) ), 0, -1, L"Quit" );

    E_GAME_STATE state = EGS_QUIT;
    bool running = true;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene();
        mGUI->drawAll();

        if( buttonQuit->isPressed() )
        {
            state = EGS_QUIT;
            running = false;
        }

        mVideoDriver->endScene();
    }

    mGUI->clear();
    return state;
}
