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

class IPlayer
{
public:
    IPlayer( ExplorePtr explore, IPlayerPtr parent );
    virtual ~IPlayer();

    virtual void update() = 0;

    irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const;

    void addOwnedItem( Item* item );

    EntityPtr getEntity() const;
    IPlayer *getParent() const;
    void setParent( IPlayerPtr parent );

protected:
    PropTreePtr mProperties;

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;

    EntityPtr mEntity;

    std::vector<Item*> mInventory;
    ItemVector mOwnedItems;

    IPlayerPtr mParent;
};

#endif // PLAYER_H
