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

#include "IPlayer.h"

#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

#include "Item.h"
#include "ItemFactory.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

IPlayer::IPlayer( ExplorePtr explore, IPlayerPtr parent )
    : NetworkSyncable(),
      mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() ),
      mParent( parent )
{
    setTypeID( ENTI_PLAYER );

    if( parent )
        parent->addChild( this );
}

IPlayer::~IPlayer()
{
}

void IPlayer::addOwnedItem( ItemPtr item )
{
    mOwnedItems.insert( std::make_pair( item.get(), item ) );
}

void IPlayer::removeOwnedItem( ItemPtr item )
{
    ItemMap::iterator x = mOwnedItems.find( item.get() );

    if( x != mOwnedItems.end() )
        mOwnedItems.erase( x );
}

ItemMap &IPlayer::getOwnedItems()
{
    return mOwnedItems;
}

IPlayerPtr IPlayer::getParent() const
{
    return mParent.lock();
}

void IPlayer::setParent( IPlayerPtr parent )
{
    mParent = parent;
}

boost::optional<IPlayerPtr> IPlayer::getChild( uint32_t uid )
{
    foreach_( IPlayerVector::value_type &x, mChildren )
    {
        if( x->getUID() == uid )
            return x;
    }
    return boost::none;
}

void IPlayer::destroyChild( uint32_t uid )
{
    for( IPlayerVector::iterator x = mChildren.begin(); x != mChildren.end(); ++x )
    {
        if( ( *x )->getUID() == uid )
        {
            mChildren.erase( x );
        }
    }
}

void IPlayer::serializeAll( const uint8_t actionID,
                            std::list<NetworkSyncablePacket> &playerList,
                            std::list<NetworkSyncablePacket> &itemList )
{
    typedef std::map<Item*,ItemPtr> itemMap_t;

    foreach_( itemMap_t::value_type &x, mOwnedItems )
    {
        itemList.push_back( x.second->serialize( actionID ) );
    }

    playerList.push_back( serialize( actionID ) );

    foreach_( IPlayerVector::value_type &x, mChildren )
    {
        x->serializeAll( actionID, playerList, itemList );
    }
}

void IPlayer::serializeInternal( NetworkSyncablePacket &/*packet*/, uint8_t /*actionID*/ )
{
}

boost::optional<NetworkSyncablePacket> IPlayer::deserializeInternal(
        NetworkSyncablePacket &/*packet*/ )
{
    return boost::none;
}

void IPlayer::addChild( IPlayer *child )
{
    mChildren.push_back( IPlayerPtr( child ) );
}
