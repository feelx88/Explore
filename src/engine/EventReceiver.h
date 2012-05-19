#ifndef EVENTRECEIVER_H
#define EVENTRECEIVER_H

#include <irrlicht/IEventReceiver.h>

class EventReceiver : public irr::IEventReceiver
{
public:
    EventReceiver();
    bool OnEvent( const irr::SEvent &event );

    bool keyPressed( const irr::EKEY_CODE &code ) const;

private:
    bool mPressed[irr::KEY_KEY_CODES_COUNT];
};

#endif // EVENTRECEIVER_H
