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


#include "ExploreMenu.h"
#include "Explore.h"
#include <engine/LoggerSingleton.h>
#include <engine/IrrlichtTools.h>

using namespace irr;
using namespace core;
using namespace gui;

enum E_GUI_IDS
{
    EGID_MAINMENU = 10,
    EGID_NEW_GAME,
    EGID_LOAD_GAME,
    EGID_CONNECT,
    EGID_OPTIONS,
    EGID_QUIT,
    EGID_OPTIONS_WINDOW = 20
};

ExploreMenu::ExploreMenu( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mGUI( mDevice->getGUIEnvironment() )
{
}

E_GAME_STATE ExploreMenu::run()
{
    E_GAME_STATE state = EGS_MAIN_MENU;

    int windowWidth = mExplore->readConfigValue<int>( "Engine.windowWidth", 640 );
    int windowHeight = mExplore->readConfigValue<int>( "Engine.windowHeight", 480 );

    //Container window
    IGUIWindow *main = mGUI->addWindow( recti( 0, 0, windowWidth, windowHeight ) );
    main->setDrawBackground( false );
    main->setDraggable( false );
    main->setDrawTitlebar( false );
    main->getCloseButton()->setVisible( false );

    mGUI->loadGUI( "data/mainUI.xml", main );

    //Center menu
    IGUIWindow *menuWindow = static_cast<IGUIWindow*>(
                main->getElementFromId( 10 ) );
    recti menurect = menuWindow->getAbsoluteClippingRect();
    menuWindow->setRelativePosition(
                vector2di( windowWidth / 2 - menurect.getWidth() / 2,
                           windowHeight / 2 - menurect.getHeight() / 2 ) );

    //Main button callbacks
    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "New Game button pressed" );
            *state = EGS_GAME;
            return true;
        }
        E_GAME_STATE *state;
    } newGameClicked;
    newGameClicked.state = &state;

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "Load Game button pressed" );
            return true;
        }
    } loadGameClicked;

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "Connect button pressed" );
            return true;
        }
    } connectClicked;

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr caller ) {
            _LOG( "Options button pressed" );
            IGUIElementPtr win = caller->getParent()->getParent()->getElementFromId(
                        EGID_OPTIONS_WINDOW );
            win->setVisible( true );
            win->setRelativePosition( position2di( 0, 0 ) );
            win->getParent()->bringToFront( win );
            return true;
        }
    } optionsClicked;

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "Quit button pressed" );
            *state = EGS_QUIT;
            return true;
        }
        E_GAME_STATE *state;
    } quitClicked;
    quitClicked.state = &state;

    mExplore->getEventReceiver()->registerGUICallback( &newGameClicked,
                                                       EGID_NEW_GAME,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &loadGameClicked,
                                                       EGID_LOAD_GAME,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &connectClicked,
                                                       EGID_CONNECT,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &optionsClicked,
                                                       EGID_OPTIONS,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &quitClicked,
                                                       EGID_QUIT,
                                                       EGET_BUTTON_CLICKED );

    //Callback to prevent closing of optionsWindow
    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr caller ) {
            _LOG( "Options Window closed" );
            caller->setVisible( false );
            return true;
        }
    } optionsWindowClose;

    mExplore->getEventReceiver()->registerGUICallback( &optionsWindowClose,
                                                       EGID_OPTIONS_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    while( state == EGS_MAIN_MENU && mDevice->run() )
    {
        mVideoDriver->beginScene( true, true,
                                  irr::video::SColor( 255, 100, 100, 100 ) );
        mGUI->drawAll();

        //Handle console key
        if( mExplore->getEventReceiver()->keyClicked( KEY_F12 ) )
        {
            mExplore->getScriptConsole()->setVisible(
                        !mExplore->getScriptConsole()->visible() );

        }

        mVideoDriver->endScene();
    }

    //Remove container
    main->remove();

    return state;
}
