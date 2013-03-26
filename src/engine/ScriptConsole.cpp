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

#include <ScriptConsole.h>
#include <IrrlichtTools.h>
#include <LoggerSingleton.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace gui;

ScriptConsole::ScriptConsole( IrrlichtDevicePtr device, PropTreePtr properties )
    : mDevice( device ),
      mGUI( mDevice->getGUIEnvironment() ),
      mProperties( properties ),
      mListIndex( 0 )
{
    recti windowSize( vector2di(),
                      mProperties->get( "Console.Size", vector2di( 640, 480 ) ) );
    recti size = windowSize;

    mMainWindow = mGUI->addWindow( size, false, L"Console" );
    mMainWindow->getCloseButton()->setVisible( false );

    size.UpperLeftCorner.X += 10;
    size.UpperLeftCorner.Y += mGUI->getSkin()->getFont()->getDimension( L"Console" ).Height + 10;
    size.LowerRightCorner.X -= 10;
    size.LowerRightCorner.Y -= 10 + 20 + 10;

    mOutputBox = mGUI->addListBox( size, mMainWindow, -1, true );
    mOutputBox->setAutoScrollEnabled( true );

    size = windowSize;
    size.UpperLeftCorner.X += 10;
    size.UpperLeftCorner.Y = windowSize.LowerRightCorner.Y - ( 10 + 20 );
    size.LowerRightCorner.X -= 10 + 40 + 10;
    size.LowerRightCorner.Y -= 10;

    mInputBox = mGUI->addEditBox( L"", size, true, mMainWindow );

    size.UpperLeftCorner.X = windowSize.LowerRightCorner.X - ( 10 + 40 );
    size.LowerRightCorner.X = windowSize.LowerRightCorner.X - 10;

    mSendButton = mGUI->addButton( size, mMainWindow, -1, L"Send" );
}

ScriptConsole::~ScriptConsole()
{
}

void ScriptConsole::setVisible( bool visible )
{
    mMainWindow->setVisible( visible );
    mGUI->getRootGUIElement()->bringToFront( mMainWindow );
    if( visible )
        mGUI->setFocus( mInputBox );
    else
        mGUI->removeFocus( mInputBox );
}

bool ScriptConsole::visible() const
{
    return mMainWindow->isVisible();
}
