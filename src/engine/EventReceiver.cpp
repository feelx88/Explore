#include "EventReceiver.h"
#include "LoggerSingleton.h"

using namespace irr;

EventReceiver::EventReceiver()
{
}

bool EventReceiver::OnEvent(const SEvent &event)
{
    if( event.EventType == EET_LOG_TEXT_EVENT )
    {
        _LOG( event.LogEvent.Text );
        return true;
    }

    return false;
}
