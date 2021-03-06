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


#include <ExploreMenu.h>
#include <Explore.h>
#include <network/ExploreServer.h>
#include <engine/LoggerSingleton.h>
#include <engine/IrrlichtTools.h>
#include <engine/NetworkTools.h>

using namespace irr;
using namespace core;
using namespace gui;

enum E_GUI_ID
{
    EGID_MAIN_WINDOW = 10,
    EGID_MAIN_NEW_GAME,
    EGID_MAIN_LOAD_GAME,
    EGID_MAIN_CONNECT,
    EGID_MAIN_OPTIONS,
    EGID_MAIN_QUIT,
    EGID_OPTIONS_WINDOW = 20,
    EGID_CONNECT_WINDOW = 30,
    EGID_CONNECT_IP,
    EGID_CONNECT_PORT,
    EGID_CONNECT_BUTTON,
    EGID_CONNECTIONSTATUS_WINDOW = 100,
    EGID_CONNECTIONSTATUS_FRAME,
    EGID_CONNECTIONSTATUS_BUTTON
};

namespace menuCallbacks
{

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

struct ConnectClickedCallback : public EventReceiver::GUICallback
{
    bool call( IGUIElementPtr caller )
    {
        IGUIElementPtr win = caller->getParent();
        IGUIEditBoxPtr ipBox = static_cast<IGUIEditBoxPtr>(
                    win->getElementFromId( EGID_CONNECT_IP, true ) );
        IGUIEditBoxPtr portBox = static_cast<IGUIEditBoxPtr>(
                    win->getElementFromId( EGID_CONNECT_PORT, true ) );

        std::string ip = core::stringc( ipBox->getText() ).c_str();
        int port = boost::lexical_cast<int>( portBox->getText() );

        StringVector resolvedIPS = NetworkTools::resolveName(
                    ip, mExplore->getIOService() );

        if( !resolvedIPS.empty() )
        {
            //Write to config file to store last used server
            mExplore->writeConfigValue<std::string>( "Server.DefaultIP", ip );
            mExplore->writeConfigValue<int>( "Server.DefaultPort", port );
            mExplore->saveConfig();

            mExplore->getExploreServer()->requestConnection(
                        resolvedIPS.at( 0 ), port );
        }
        return true;
    }
    ExplorePtr mExplore;
};

}

ExploreMenu::ExploreMenu( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mGUI( mDevice->getGUIEnvironment() )
{
}

E_GAME_STATE ExploreMenu::run()
{
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
    video::SColor oldBGColor = mGUI->getSkin()->getColor( EGDC_3D_FACE );
    video::SColor newBGColor = oldBGColor;
    newBGColor.setAlpha( 255 );
    mGUI->getSkin()->setColor( EGDC_3D_FACE, newBGColor );

    //Center menu and windows
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_MAIN_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_OPTIONS_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_CONNECT_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );
    IrrlichtTools::guiCenterElement( main->getElementFromId( EGID_CONNECTIONSTATUS_WINDOW ),
                                     windowWidth / 2, windowHeight / 2 );

    //Hide windows
    main->getElementFromId( EGID_OPTIONS_WINDOW )->setVisible( false );
    main->getElementFromId( EGID_CONNECT_WINDOW )->setVisible( false );
    main->getElementFromId( EGID_CONNECTIONSTATUS_WINDOW )->setVisible( false );

    //Set ip/port from config file
    {
        IGUIEditBoxPtr ipBox = static_cast<IGUIEditBoxPtr>(
                    main->getElementFromId( EGID_CONNECT_IP, true ) );
        IGUIEditBoxPtr portBox = static_cast<IGUIEditBoxPtr>(
                    main->getElementFromId( EGID_CONNECT_PORT, true ) );
        std::string defaultIP = mExplore->readConfigValue<std::string>(
                    "Server.DefaultIP", "127.0.0.1" );
        std::string defaultPort = mExplore->readConfigValue<std::string>(
                    "Server.DefaultPort", "6556" );
        ipBox->setText( core::stringw( defaultIP.c_str() ).c_str() );
        portBox->setText( core::stringw( defaultPort.c_str() ).c_str() );
    }

    //Main button callbacks
    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "New Game button pressed" );
            explore->setGameState( EGS_GAME );
            explore->getExploreServer()->setServerMode( true );
            return true;
        }
        ExplorePtr explore;
    } newGameClicked;
    newGameClicked.explore = mExplore;

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "Load Game button pressed" );
            return true;
        }
    } loadGameClicked;

    menuCallbacks::WindowOpenCallback connectClicked( mGUI, EGID_CONNECT_WINDOW );

    menuCallbacks::WindowOpenCallback optionsClicked( mGUI, EGID_OPTIONS_WINDOW );

    struct : public EventReceiver::GUICallback {
        bool call( IGUIElementPtr ) {
            _LOG( "Quit button pressed" );
            explore->setGameState( EGS_QUIT );
            return true;
        }
        ExplorePtr explore;
    } quitClicked;
    quitClicked.explore = mExplore;

    mExplore->getEventReceiver()->registerGUICallback( &newGameClicked,
                                                       EGID_MAIN_NEW_GAME,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &loadGameClicked,
                                                       EGID_MAIN_LOAD_GAME,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &connectClicked,
                                                       EGID_MAIN_CONNECT,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &optionsClicked,
                                                       EGID_MAIN_OPTIONS,
                                                       EGET_BUTTON_CLICKED );
    mExplore->getEventReceiver()->registerGUICallback( &quitClicked,
                                                       EGID_MAIN_QUIT,
                                                       EGET_BUTTON_CLICKED );

    //Callbacks used to prevent closing of windows
    menuCallbacks::WindowCloseCallback optionsWindowClose;
    menuCallbacks::WindowCloseCallback connectWindowClose;
    menuCallbacks::WindowCloseCallback connectionstatusWindowClose;

    mExplore->getEventReceiver()->registerGUICallback( &connectWindowClose,
                                                       EGID_CONNECT_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    mExplore->getEventReceiver()->registerGUICallback( &optionsWindowClose,
                                                       EGID_OPTIONS_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    mExplore->getEventReceiver()->registerGUICallback( &connectionstatusWindowClose,
                                                       EGID_CONNECTIONSTATUS_WINDOW,
                                                       EGET_ELEMENT_CLOSED );

    menuCallbacks::ConnectClickedCallback connectButtonClicked;
    connectButtonClicked.mExplore = mExplore;
    mExplore->getEventReceiver()->registerGUICallback( &connectButtonClicked,
                                                       EGID_CONNECT_BUTTON,
                                                       EGET_BUTTON_CLICKED );

    bool connected = false;
    int connectionstatusFrameLength;
    {
        core::recti rect =
                main->getElementFromId( EGID_CONNECTIONSTATUS_FRAME, true )->getAbsoluteClippingRect();
        connectionstatusFrameLength = rect.LowerRightCorner.X- rect.UpperLeftCorner.X;
    }

    //Main loop
    while( mExplore->getGameState() == EGS_MAIN_MENU && mDevice->run() )
    {
        mVideoDriver->beginScene( true, true,
                                  irr::video::SColor( 255, 100, 100, 100 ) );
        mGUI->drawAll();

        if( mExplore->getExploreServer()->hasConnection() && !connected )
        {
            connected = true;
            main->getElementFromId( EGID_MAIN_WINDOW )->setVisible( false );
            main->getElementFromId( EGID_CONNECT_WINDOW )->setVisible( false );
            main->getElementFromId( EGID_CONNECTIONSTATUS_WINDOW )->setVisible( true );
        }

        if( connected )
        {
            int length = (float)connectionstatusFrameLength
                    * mExplore->getExploreServer()->getConnectionProgress();
            IGUIButtonPtr button = static_cast<IGUIButtonPtr>(
                        main->getElementFromId( EGID_CONNECTIONSTATUS_BUTTON, true ) );
            button->setMinSize( core::dimension2du( length, 10 ) );

            if( mExplore->getExploreServer()->isInitialized() )
            {
                mExplore->setGameState( EGS_GAME );
            }
        }

        mVideoDriver->endScene();
        mExplore->getIOService()->poll();
    }

    //Remove container
    main->remove();

    //Reset window background
    mGUI->getSkin()->setColor( EGDC_3D_FACE, oldBGColor );

    return EGS_MAIN_MENU;
}
