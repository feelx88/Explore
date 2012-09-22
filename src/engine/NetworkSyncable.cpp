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

#include "NetworkSyncable.h"
#include <sstream>
#include <iostream>

std::map<uint32_t, NetworkSyncable*> NetworkSyncable::sUIDMap;

NetworkSyncable::NetworkSyncable()
    : mUID( 0 ),
      mTypeID( 0 )
{
    setUID( nextUID() );
}

NetworkSyncable::NetworkSyncable( uint32_t uid , uint8_t typeID )
    : mUID( 0 ),
      mTypeID( typeID )
{
    setUID( uid );
}

NetworkSyncable::~NetworkSyncable()
{
    sUIDMap.erase( mUID );
}

boost::optional<NetworkSyncablePacket> NetworkSyncable::deserialize( NetworkSyncablePacket &packet )
{
    return deserializeInternal( packet );
}

NetworkSyncablePacket NetworkSyncable::serialize( uint8_t actionID )
{
    NetworkSyncablePacket packet( mUID, mTypeID, actionID, "" );
    serializeInternal( packet, actionID );
    return packet;
}

NetworkSyncable *NetworkSyncable::getObject( uint32_t uid )
{
    std::map<uint32_t, NetworkSyncable*>::iterator x = sUIDMap.find( uid );

    if( x != sUIDMap.end() )
        return x->second;
    else
        return 0;
}

void NetworkSyncable::setUID( uint32_t uid )
{
    if( mUID == uid )
        return;

    if( sUIDMap.find( uid ) != sUIDMap.end() )
        throw new UIDAlreadyUsedException( uid );

    if( mUID > 0 )
        sUIDMap.erase( mUID );

    sUIDMap.insert( std::make_pair( uid, this ) );
    mUID = uid;
}

uint32_t NetworkSyncable::nextUID()
{
    if( sUIDMap.empty() )
        return 1;

    uint32_t uid = ( --sUIDMap.end() )->first;
    uid++;
    return uid;
}


NetworkSyncable::UIDAlreadyUsedException::UIDAlreadyUsedException( uint32_t uid )
{
    std::cerr << "UID alredy used: " << uid << std::endl;
}
