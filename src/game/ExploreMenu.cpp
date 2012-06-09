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


#include "ExploreMenu.h"
#include "Explore.h"
#include <engine/LoggerSingleton.h>
#include <engine/IrrlichtTools.h>

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
    mDevice->setWindowCaption( L"Explore - Main Menu" );
    int windowWidth = mExplore->readConfigValue<int>( "Engine.windowWidth", 640 );
    int windowHeight = mExplore->readConfigValue<int>( "Engine.windowHeight", 480 );

    IGUIButton *buttonNewGame = IrrlichtTools::guiCenterFittingButton(
                mGUI, L"New Game", windowWidth / 2, windowHeight * 1 / 5 );

    IGUIButton *buttonLoadGame = IrrlichtTools::guiCenterFittingButton(
                mGUI, L"Load Game", windowWidth / 2, windowHeight * 2 / 5 );

    IGUIButton *buttonOptions = IrrlichtTools::guiCenterFittingButton(
                mGUI, L"Options", windowWidth / 2, windowHeight * 3 / 5 );

    IGUIButton *buttonQuit = IrrlichtTools::guiCenterFittingButton(
                mGUI, L"Quit", windowWidth / 2, windowHeight * 4 / 5 );

    E_GAME_STATE state = EGS_MAIN_MENU;
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

        if( buttonQuit->isPressed() ||
                mExplore->getEventReceiver()->keyClicked( KEY_ESCAPE ) )
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
