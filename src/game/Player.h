#ifndef PLAYER_H
#define PLAYER_H

#include <engine/Entity.h>
#include "Explore.h"
#include <vector>

typedef std::vector<ItemPtr> ItemVector;

class Player
{
public:

    Player( ExplorePtr explore );
    virtual ~Player();

    EntityPtr getEntity() const;

    ItemPtr getActiveItem() const;

    void update();

    irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const;

private:

    void processControls();
    void drawCrosshair();

    PropTreePtr mProperties;

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;
    ICameraSceneNodePtr mCamera;

    EntityPtr mEntity;
    ItemVector mInventory;
    int mActiveItem;

    int mCrossX, mCrossY;
    irr::video::SColor mCrossColor;

    IGUIElementPtr mItemWin;
};

#endif // PLAYER_H
