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


#include "EventReceiver.h"
#include "LoggerSingleton.h"
#include "PythonTools.h"

using namespace irr;

EventReceiver::EventReceiver()
    : mMouseLocked( false ),
      mMouseX( 0 ),
      mMouseY( 0 ),
      mMouseMoveX( 0 ),
      mMouseMoveY( 0 ),
      mWinWidth( 0 ),
      mWinHeight( 0 )
{
    for( unsigned int x = 0; x < irr::KEY_KEY_CODES_COUNT; ++x )
    {
        mPressed[x] = false;
        mClicked[x] = false;
    }

    for( unsigned int x = 0; x < 3; ++x )
    {
        mMousePressed[x] = false;
        mMouseClicked[x] = false;
    }
}

void EventReceiver::setDevice( IrrlichtDevicePtr device )
{
    mDevice = device;

    irr::core::dimension2du size = mDevice->getVideoDriver()->getScreenSize();

    mWinWidth = size.Width;
    mWinHeight = size.Height;
}

void EventReceiver::setScriptConsole( ScriptConsolePtr console )
{
    mScriptConsole = console;

    LoggerSingleton::instance().setScriptConsole( console );
}

bool EventReceiver::OnEvent( const SEvent &event )
{
    if( !mDevice )
        return false;

    mMouseMoveX = 0;
    mMouseMoveY = 0;

    if( event.EventType == EET_LOG_TEXT_EVENT )
    {
        _LOG( event.LogEvent.Text );
        return true;
    }

    if( event.EventType == EET_KEY_INPUT_EVENT )
    {
        boost::unordered::unordered_map<EKEY_CODE, KeyCallback*>::iterator x =
                mKeyCallbacks.find( event.KeyInput.Key );

        if( x != mKeyCallbacks.end() )
        {
            if( x->second->call( event.KeyInput ) )
                return true;
        }

        if( mScriptConsole &&
                mDevice->getGUIEnvironment()->getFocus() != mScriptConsole->mInputBox )
        {
            mPressed[event.KeyInput.Key] = event.KeyInput.PressedDown;
            mClicked[event.KeyInput.Key] = !event.KeyInput.PressedDown;
        }
        else if( mScriptConsole )
        {
            unsigned int &lIndex= mScriptConsole->mListIndex;
            std::vector<std::string> &hist = mScriptConsole->mHistory;

            if( event.KeyInput.Key == KEY_RETURN && !event.KeyInput.PressedDown
                    && core::stringw( mScriptConsole->mInputBox->getText() ).size() > 0 )
                sendScriptConsoleCommand();
            if( event.KeyInput.Key == KEY_UP && !event.KeyInput.PressedDown
                    && !hist.empty() )
            {
                if( lIndex > 0 )
                    lIndex--;
                irr::core::stringw string( hist.at( lIndex ).c_str() );
                mScriptConsole->mInputBox->setText( string.c_str() );
            }
            if( event.KeyInput.Key == KEY_DOWN && !event.KeyInput.PressedDown )
            {
                if( hist.empty() || lIndex >= hist.size() - 1 )
                    mScriptConsole->mInputBox->setText( L"" );
                else if( lIndex < hist.size() - 1 )
                {
                    lIndex++;
                    irr::core::stringw string( hist.at( lIndex ).c_str() );
                    mScriptConsole->mInputBox->setText( string.c_str() );
                }
            }
        }
    }

    if( event.EventType == EET_MOUSE_INPUT_EVENT )
    {
        if( event.MouseInput.Event == EMIE_LMOUSE_LEFT_UP )
            mMouseClicked[0] = true;
        else if( event.MouseInput.Event == EMIE_RMOUSE_LEFT_UP )
            mMouseClicked[1] = true;
        else if( event.MouseInput.Event == EMIE_MMOUSE_LEFT_UP )
            mMouseClicked[2] = true;

        mMousePressed[0] = event.MouseInput.isLeftPressed();
        mMousePressed[1] = event.MouseInput.isRightPressed();
        mMousePressed[2] = event.MouseInput.isMiddlePressed();

        if( mMouseLocked &&
                event.MouseInput.X == mWinWidth / 2 &&
                event.MouseInput.Y == mWinHeight / 2 )
        {
            return false;
        }

        mMouseMoveX = event.MouseInput.X - mMouseX;
        mMouseMoveY = event.MouseInput.Y - mMouseY;

        if( mMouseLocked )
        {
            mMouseX = mWinWidth / 2;
            mMouseY = mWinHeight / 2;

            mDevice->getCursorControl()->setPosition( mMouseX, mMouseY );
        }
        else
        {
            mMouseX = event.MouseInput.X;
            mMouseY = event.MouseInput.Y;
        }

        mMouseWheelY = event.MouseInput.Wheel;
    }

    if( event.EventType == EET_GUI_EVENT )
    {
        boost::unordered::unordered_map<std::pair<int, irr::gui::EGUI_EVENT_TYPE>,
            GUICallback*>::iterator x = mGUICallbacks.find( std::make_pair(
                                                                event.GUIEvent.Caller->getID(), event.GUIEvent.EventType ) );

        if( x != mGUICallbacks.end() )
        {
            if( x->second->call( event.GUIEvent.Caller ) )
                return true;
        }

        if( event.GUIEvent.EventType == gui::EGET_BUTTON_CLICKED )
        {
            if( mScriptConsole &&
                    event.GUIEvent.Caller == mScriptConsole->mSendButton )
                sendScriptConsoleCommand();
        }
    }

    return false;
}

bool EventReceiver::keyPressed( const EKEY_CODE &code ) const
{
    return mPressed[code];
}

bool EventReceiver::keyClicked( const EKEY_CODE &code )
{
    bool click = mClicked[code];
    mClicked[code] = false;
    return click;
}

bool EventReceiver::mousePressed( const unsigned int &buttonNum ) const
{
    if( buttonNum > 3 )
        return false;

    return mMousePressed[buttonNum];
}

bool EventReceiver::mouseClicked( const unsigned int &buttonNum )
{
    if( buttonNum > 3 )
        return false;

    bool click = mMouseClicked[buttonNum];
    mMouseClicked[buttonNum] = false;
    return click;
}

void EventReceiver::lockMouse( bool lock, bool changeVisibility )
{
    mMouseLocked = lock;

    for( int x = 0; x < 3; ++x )
    {
        mMouseClicked[x] = false;
        mMousePressed[x] = false;
    }

    if( changeVisibility )
        mDevice->getCursorControl()->setVisible( !lock );

    if( lock )
    {
        mMouseX = mWinWidth / 2;
        mMouseY = mWinHeight / 2;

        mDevice->getCursorControl()->setPosition( mMouseX, mMouseY );
    }
}


bool EventReceiver::isMouseLocked() const
{
    return mMouseLocked;
}

int EventReceiver::mouseX() const
{
    return mMouseX;
}

int EventReceiver::mouseY() const
{
    return mMouseY;
}

int EventReceiver::mouseMoveX() const
{
    return mMouseMoveX;
}

int EventReceiver::mouseMoveY() const
{
    return mMouseMoveY;
}

int EventReceiver::mouseWheelY()
{
    return mMouseWheelY;
    mMouseWheelY = 0;
}

void EventReceiver::registerGUICallback( EventReceiver::GUICallback *callback,
                                         int id, gui::EGUI_EVENT_TYPE evt )
{
    removeGUICallback( id, evt );
    mGUICallbacks.insert( std::make_pair( std::make_pair( id, evt ), callback ) );
}

void EventReceiver::removeGUICallback( int id, gui::EGUI_EVENT_TYPE evt )
{
    boost::unordered::unordered_map<std::pair<int, irr::gui::EGUI_EVENT_TYPE>,
        GUICallback*>::iterator x = mGUICallbacks.find( std::make_pair( id, evt ) );

    if( x != mGUICallbacks.end() )
        mGUICallbacks.erase( x );
}

void EventReceiver::registerKeyCallback(EventReceiver::KeyCallback *callback, EKEY_CODE evt)
{
    mKeyCallbacks.insert( std::make_pair( evt, callback ) );
}

void EventReceiver::removeKeyCallback(EKEY_CODE evt)
{
    mKeyCallbacks.erase( evt );
}

void EventReceiver::sendScriptConsoleCommand()
{
    std::string script(
                irr::core::stringc( mScriptConsole->mInputBox->getText() ).c_str() );
    PythonTools::execString( script );

    mScriptConsole->mHistory.push_back( script );

    mScriptConsole->mInputBox->setText( L"" );
    mScriptConsole->mOutputBox->setSelected( -1 );
    mScriptConsole->mListIndex = mScriptConsole->mHistory.size();
}
