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

std::map<uint32_t, NetworkSyncable*> NetworkSyncable::sUIDMap;

NetworkSyncable::NetworkSyncable()
    : mUID( 0 )
{
    setUID( nextUID() );
}

NetworkSyncable::NetworkSyncable( uint32_t uid )
    : mUID( 0 )
{
    setUID( uid );
}

NetworkSyncable::~NetworkSyncable()
{
    sUIDMap.erase( mUID );
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
    mUID = uid;
}

const char *NetworkSyncable::UIDAlreadyUsedException::what()
{
    std::stringstream str;
    str << "UID alredy used: " << mUID;
    return str.str().c_str();
}
