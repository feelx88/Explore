#ifndef SIMPLEFORCEGUNITEM_H
#define SIMPLEFORCEGUNITEM_H

#include "../Item.h"

class SimpleForceGunItem : public Item
{
public:
    SimpleForceGunItem( ExplorePtr explore, PlayerPtr owner );

    void startAction( int actionID );

private:
    void shoot( bool forward );

    float mRayDistance;
    float mForceMultiplicator;
};

#endif // SIMPLEFORCEGUNITEM_H
