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

#include "WorldPlayer.h"
#include "../Item.h"
#include <boost/foreach.hpp>

WorldPlayer::WorldPlayer( ExplorePtr explore )
    : IPlayer( explore, IPlayerPtr() )
{
}

void WorldPlayer::serializeAll( const uint8_t actionID,
                                std::list<NetworkSyncablePacket> &list )
{
    typedef std::map<Item*,ItemPtr> itemMap_t;

    foreach_( itemMap_t::value_type &x, mOwnedItems )
    {
        list.push_back( x.second->serialize( actionID ) );
    }

    if( !mLocalPlayer )
        return;

    list.push_back( mLocalPlayer->serialize( actionID ) );

    foreach_( itemMap_t::value_type &x, mLocalPlayer->getOwnedItems() )
    {
        list.push_back( x.second->serialize( actionID ) );
    }
}

void WorldPlayer::update()
{
    for( ItemMap::iterator x = mOwnedItems.begin(); x != mOwnedItems.end(); ++x )
    {
        x->second->startAction( EAID_UPDATE );
    }

    for( std::vector<IPlayer*>::iterator x = mChildren.begin();
         x != mChildren.end(); ++x )
        ( *x )->update();

    //TODO: If Networkmessenger->hasPacketsInQueue => foreach packet
    //=> if typeid==item => request item data => create item
}

void WorldPlayer::setLocalPlayer(IPlayerPtr player)
{
    mLocalPlayer = player;
}

IPlayerPtr WorldPlayer::getLocalPlayer() const
{
    return mLocalPlayer;
}

irr::core::vector3df WorldPlayer::rotateToDirection(irr::core::vector3df dir) const
{
    return dir;
}

irr::core::vector3df WorldPlayer::getPosition() const
{
    return irr::core::vector3df();
}

irr::core::quaternion WorldPlayer::getRotation() const
{
    return irr::core::quaternion();
}
