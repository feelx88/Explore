#include "ExploreMenu.h"
#include "Explore.h"
#include <engine/LoggerSingleton.h>
#include <engine/GUITools.h>

using namespace irr;
using namespace core;
using namespace gui;

ExploreMenu::ExploreMenu( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mGUI( mDevice->getGUIEnvironment() )
{
}

E_GAME_STATE ExploreMenu::run()
{
    int windowWidth = mExplore->readConfigValue<int>( "Engine.windowWidth", 640 );
    int windowHeight = mExplore->readConfigValue<int>( "Engine.windowHeight", 480 );

    IGUIButton *buttonNewGame = GUITools::centerFittingButton(
                mGUI, L"New Game", windowWidth / 2, windowHeight * 1 / 5 );

    IGUIButton *buttonLoadGame = GUITools::centerFittingButton(
                mGUI, L"Load Game", windowWidth / 2, windowHeight * 2 / 5 );

    IGUIButton *buttonOptions = GUITools::centerFittingButton(
                mGUI, L"Options", windowWidth / 2, windowHeight * 3 / 5 );

    IGUIButton *buttonQuit = GUITools::centerFittingButton(
                mGUI, L"Quit", windowWidth / 2, windowHeight * 4 / 5 );

    E_GAME_STATE state = EGS_QUIT;
    bool running = true;

    while( running && mDevice->run() )
    {
        mVideoDriver->beginScene( true, true, irr::video::SColor( 255, 100, 100, 100 ) );
        mGUI->drawAll();

        //Dummy buttons
        if( buttonLoadGame->isPressed() )
            _LOG( "Load Game button pressed" );
        if( buttonOptions->isPressed() )
            _LOG( "Options button pressed" );

        //Working buttons
        if( buttonNewGame->isPressed() )
        {
            _LOG( "New Game button pressed" );
            state = EGS_GAME;
            running = false;
        }

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
