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

enum E_GUI_ID
{
    EGID_MAINMENU = 10,
    EGID_NEW_GAME,
    EGID_LOAD_GAME,
    EGID_CONNECT,
    EGID_OPTIONS,
    EGID_QUIT,
    EGID_OPTIONS_WINDOW = 20,
    EGID_CONNECT_WINDOW = 30,
    EGID_CONNECT_IP,
    EGID_CONNECT_PORT,
    EGID_CONNECT_BUTTON
};

struct WindowOpenCallback : EventReceiver::GUICallback
{
    WindowOpenCallback( IGUIEnvironmentPtr env, E_GUI_ID id )
        : mGUI( env ),
          mWindowID( id )
    {}

    bool call( IGUIElementPtr caller )
    {
        IGUIElementPtr win = caller->getParent()->getParent()->getElementFromId(
                    mWindowID );

        if( !win )
            return false;

        win->setVisible( true );
        win->getParent()->bringToFront( win );
        mGUI->setFocus( win );
        return true;
    }

    IGUIEnvironmentPtr mGUI;
    E_GUI_ID mWindowID;
};

struct WindowCloseCallback : public EventReceiver::GUICallback
{
    bool call( IGUIElementPtr caller )
    {
        caller->setVisible( false );
        return true;
    }
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

    int windowWidth = mExplore->readConfigValue<int>( "Engine.WindowWidth", 640 );
    int windowHeight = mExplore->readConfigValue<int>( "Engine.WindowHeight", 480 );

    //Container window
    IGUIWindow *main = mGUI->addWindow( recti( 0, 0, windowWidth, windowHeight ) );
    main->setDrawBackground( false );
    main->setDraggable( false );
    main->setDrawTitlebar( false );
    main->getCloseButton()->setVisible( false );

    //Load gui and make windows opaque
    mGUI->loadGUI( "data/mainUI.xml", main );
    video::SColor bgColor = mGUI->getSkin()->getColor( EGDC_3D_FACE );
    bgColor.setAlpha( 255 );
    mGUI->getSkin()->setColor( EGDC_3D_FACE, bgColor );

    //Center menu and windows
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_MAINMENU ),
                                     windowWidth / 2, windowHeight / 2 );
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_OPTIONS_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_CONNECT_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );

    //Hide windows
    main->getElementFromId( EGID_OPTIONS_WINDOW )->setVisible( false );
    main->getElementFromId( EGID_CONNECT_WINDOW )->setVisible( false );

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

    WindowOpenCallback connectClicked( mGUI, EGID_CONNECT_WINDOW );

    WindowOpenCallback optionsClicked( mGUI, EGID_OPTIONS_WINDOW );

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

    //Callbacks used to prevent closing of windows
    WindowCloseCallback optionsWindowClose;

    WindowCloseCallback connectWindowClose;

    mExplore->getEventReceiver()->registerGUICallback( &connectWindowClose,
                                                       EGID_CONNECT_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    mExplore->getEventReceiver()->registerGUICallback( &optionsWindowClose,
                                                       EGID_OPTIONS_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    //Main loop
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
