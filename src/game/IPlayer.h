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

class IPlayer;
typedef boost::shared_ptr<IPlayer> IPlayerPtr;
typedef boost::weak_ptr<IPlayer> IPlayerWeakPtr;

typedef boost::unordered::unordered_map<Item*,ItemPtr> ItemMap;
typedef std::vector<IPlayer*> IPlayerVector;

class IPlayer : public boost::enable_shared_from_this<IPlayer>,
        public NetworkSyncable
{
public:
    IPlayer( ExplorePtr explore, IPlayerPtr parent );
    virtual ~IPlayer();

    virtual void update() = 0;

    virtual irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const = 0;
    virtual irr::core::vector3df getPosition() const = 0;
    virtual irr::core::quaternion getRotation() const = 0;

    virtual void addOwnedItem( ItemPtr item );
    virtual void removeOwnedItem( ItemPtr item );
    ItemMap &getOwnedItems();

    IPlayer *getParent() const;
    void setParent( IPlayerPtr parent );

    virtual void serializeAll( const uint8_t actionID,
                               std::list<NetworkSyncablePacket> &list );

protected:

    virtual void serializeInternal( NetworkSyncablePacket &packet,
                                    uint8_t actionID );
    virtual boost::optional<NetworkSyncablePacket> deserializeInternal(
            NetworkSyncablePacket &packet );

    PropTreePtr mProperties;

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    EventReceiverPtr mEventReceiver;
    BulletWorldPtr mBulletWorld;

    ItemMap mOwnedItems;

    IPlayer* mParent;
    IPlayerVector mChildren;
};

#endif // PLAYER_H
