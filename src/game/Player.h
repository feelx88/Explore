#ifndef PLAYER_H
#define PLAYER_H

#include <engine/EngineTypedefs.h>
#include <engine/Entity.h>
#include "Explore.h"

class Player
{
public:
    Player( ExplorePtr explore );
    virtual ~Player();

    EntityPtr getEntity() const;

    void update();

private:
    PropTreePtr mProperties;

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;
    ICameraSceneNodePtr mCamera;

    EntityPtr mEntity;

    EntityPtr mBullets[10];
    int mCurBullet;

    int mCrossX, mCrossY;
};

#endif // PLAYER_H
