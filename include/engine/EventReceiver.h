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


#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <EngineTypedefs.h>
#include <ScriptConsole.h>
#include <boost/unordered_map.hpp>

class APIEXPORT EventReceiver : public irr::IEventReceiver
{
public:
    struct GUICallback
    {
        virtual bool call( IGUIElementPtr caller ) = 0;
    };

    struct KeyCallback
    {
        virtual bool call( irr::SEvent::SKeyInput evt ) = 0;
    };

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
    int mouseMoveX();
    int mouseMoveY();

    int mouseWheelY();

    void registerGUICallback( GUICallback *callback, int id,
                              irr::gui::EGUI_EVENT_TYPE evt );
    void removeGUICallback( int id, irr::gui::EGUI_EVENT_TYPE evt );

    void registerKeyCallback( KeyCallback *callback, irr::EKEY_CODE evt );
    void removeKeyCallback( irr::EKEY_CODE evt );

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

    boost::unordered::unordered_map<std::pair<int, irr::gui::EGUI_EVENT_TYPE>,
        GUICallback*>
        mGUICallbacks;

    boost::unordered::unordered_map<irr::EKEY_CODE, KeyCallback*> mKeyCallbacks;
};

#endif // EVENTRECEIVER_H
