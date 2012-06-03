#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include "EngineTypedefs.h"

class EventReceiver : public irr::IEventReceiver
{
public:
    EventReceiver();

    void setDevice( IrrlichtDevicePtr device );

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

private:
    IrrlichtDevicePtr mDevice;

    bool mPressed[irr::KEY_KEY_CODES_COUNT];
    bool mClicked[irr::KEY_KEY_CODES_COUNT];
    bool mMousePressed[3];
    bool mMouseClicked[3];
    bool mMouseLocked;
    int mMouseX, mMouseY, mMouseMoveX, mMouseMoveY;
    int mWinWidth, mWinHeight;
};

#endif // EVENTRECEIVER_H
