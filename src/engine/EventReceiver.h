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


#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include "EngineTypedefs.h"
#include "ScriptConsole.h"

class EventReceiver : public irr::IEventReceiver
{
public:
    EventReceiver();

    void setDevice( IrrlichtDevicePtr device );
    void setScriptConsole( ScriptConsolePtr console );

    bool OnEvent( const irr::SEvent &event );

    bool keyPressed( const irr::EKEY_CODE &code ) const;
    bool keyClicked( const irr::EKEY_CODE &code );

    bool mousePressed( const unsigned int &buttonNum ) const;
    bool mouseClicked( const unsigned int &buttonNum );

    void lockMouse( bool lock, bool changeVisibility = true );
    bool isMouseLocked() const;

    int mouseX() const;
    int mouseY() const;
    int mouseMoveX() const;
    int mouseMoveY() const;

    int mouseWheelY();

private:
    void sendScriptConsoleCommand();

    IrrlichtDevicePtr mDevice;
    ScriptConsolePtr mScriptConsole;

    bool mPressed[irr::KEY_KEY_CODES_COUNT];
    bool mClicked[irr::KEY_KEY_CODES_COUNT];
    bool mMousePressed[3];
    bool mMouseClicked[3];
    bool mMouseLocked;
    int mMouseX, mMouseY, mMouseMoveX, mMouseMoveY, mMouseWheelY;
    int mWinWidth, mWinHeight;
};

#endif // EVENTRECEIVER_H
