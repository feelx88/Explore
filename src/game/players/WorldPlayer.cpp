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

WorldPlayer::WorldPlayer( ExplorePtr explore )
    : IPlayer( explore, IPlayerPtr() )
{
}

void WorldPlayer::sendUpdates( NetworkMessengerPtr messenger,
                               boost::asio::ip::udp::endpoint endpoint )
{
    for( ItemMap::iterator x = mOwnedItems.begin(); x != mOwnedItems.end(); ++x )
    {
        messenger->sendTo( x->second->serialize( 0 ), endpoint );
    }

    if( !mLocalPlayer )
        return;

    messenger->sendTo( mLocalPlayer->serialize( 0 ), endpoint );

    for( ItemMap::iterator x = mLocalPlayer->getOwnedItems().begin();
         x != mLocalPlayer->getOwnedItems().end(); ++x )
    {
        messenger->sendTo( x->second->serialize( 0 ), endpoint );
    }
}

void WorldPlayer::update()
{
    for( ItemMap::iterator x = mOwnedItems.begin(); x != mOwnedItems.end(); ++x )
    {
        x->second->startAction( EIA_UPDATE_ACTION );
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
