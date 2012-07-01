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


#ifndef PLAYER_H
#define PLAYER_H

#include <engine/Entity.h>
#include "Explore.h"
#include <vector>

typedef std::vector<ItemPtr> ItemVector;

class Player
{
public:

    enum E_PLAYER_KEY_MAPPINGS
    {
        EPKM_FORWARD = 0,
        EPKM_BACKWARD,
        EPKM_LEFT,
        EPKM_RIGHT,
        EPKM_JUMP,
        EPKM_MOUSECONTROL,
        EPKM_NEXTSLOT,
        EPKM_PREVIOUSSLOT,
        EPKM_COUNT
    };

    Player( ExplorePtr explore );
    virtual ~Player();

    EntityPtr getEntity() const;

    Item *getActiveItem() const;

    void update();

    irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const;

    void addOwnedItem( Item* item );

    void switchItem( int index );

private:

    void addItems();
    void createGUI();
    void setKeyMappings();
    void processControls();
    void drawCrosshair();

    PropTreePtr mProperties;

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;
    ICameraSceneNodePtr mCamera;

    irr::EKEY_CODE mKeyMapping[EPKM_COUNT];

    EntityPtr mEntity;
    std::vector<Item*> mInventory;
    ItemVector mOwnedItems;
    int mActiveItem, mNumItems;

    int mCrossX, mCrossY;
    irr::video::SColor mCrossColor;

    IGUIElementPtr mItemWin;
    irr::gui::IGUIButton *mItemIcons[10];

    bool mJumped;
};

#endif // PLAYER_H
