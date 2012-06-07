#include "EventReceiver.h"
#include "LoggerSingleton.h"

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
        mPressed[event.KeyInput.Key] = event.KeyInput.PressedDown;
        mClicked[event.KeyInput.Key] = !event.KeyInput.PressedDown;
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
