#include "EventReceiver.h"
#include "LoggerSingleton.h"

using namespace irr;

EventReceiver::EventReceiver()
{
    for( unsigned int x = 0; x < irr::KEY_KEY_CODES_COUNT; ++x )
    {
        mPressed[x] = false;
        mClicked[x] = false;
    }
}

bool EventReceiver::OnEvent( const SEvent &event )
{
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

    return false;
}

bool EventReceiver::keyPressed(const EKEY_CODE &code) const
{
    return mPressed[code];
}

bool EventReceiver::keyClicked(const EKEY_CODE &code)
{
    bool click = mClicked[code];
    mClicked[code] = false;
    return click;
}
