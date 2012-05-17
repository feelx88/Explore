#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <irrlicht/IEventReceiver.h>

class EventReceiver : public irr::IEventReceiver
{
public:
    EventReceiver();
    bool OnEvent( const irr::SEvent &event );
};

#endif // EVENTRECEIVER_H
