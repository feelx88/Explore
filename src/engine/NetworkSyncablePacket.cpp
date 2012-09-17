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

#include "NetworkSyncablePacket.h"
#include <sstream>

NetworkSyncablePacket::NetworkSyncablePacket( const std::string &data )
{
    std::stringstream stream;
    stream << data;

    stream.read( reinterpret_cast<char*>( &mUID ), sizeof( uint32_t ) );
    stream.read( reinterpret_cast<char*>( &mActionID ), sizeof( uint8_t ) );
    stream.read( reinterpret_cast<char*>( &mTypeID ), sizeof( uint8_t ) );
    stream.read( reinterpret_cast<char*>( &mBodySize ), sizeof( uint32_t ) );

    std::string buf;
    buf.resize( mBodySize );
    stream.read( &buf[0], mBodySize );

    mBody.write( buf.c_str(), mBodySize );
}

NetworkSyncablePacket::NetworkSyncablePacket( uint32_t uid, uint8_t typeID,
                                              uint8_t actionID,
                                              const std::string &body )
    : mUID( uid ),
      mTypeID( typeID ),
      mActionID( actionID ),
      mBodySize( body.length() )
{
    mBody.write( body.c_str(), mBodySize );
}

NetworkSyncablePacket::NetworkSyncablePacket( const NetworkSyncablePacket &other )
    : mUID( other.getUID() ),
      mTypeID( other.getTypeID() ),
      mActionID( other.getActionID() ),
      mBodySize( other.getBodySize() )
{
    mBody.write( other.getBody().c_str(), other.getBodySize() );
}

uint32_t NetworkSyncablePacket::getUID() const
{
    return mUID;
}

uint8_t NetworkSyncablePacket::getTypeID() const
{
    return mTypeID;
}

uint8_t NetworkSyncablePacket::getActionID() const
{
    return mActionID;
}

uint32_t NetworkSyncablePacket::getBodySize() const
{
    return mBodySize;
}

std::string NetworkSyncablePacket::getBody() const
{
    return mBody.str();
}

bool NetworkSyncablePacket::isValid()
{
    return mValid;
}

std::string NetworkSyncablePacket::serialize() const
{
    std::stringstream stream;

    stream.write( reinterpret_cast<const char*>( &mUID ), sizeof( uint32_t ) );
    stream.write( reinterpret_cast<const char*>( &mActionID ), sizeof( uint8_t ) );
    stream.write( reinterpret_cast<const char*>( &mTypeID ), sizeof( uint8_t ) );
    stream.write( reinterpret_cast<const char*>( &mBodySize ), sizeof( uint32_t ) );

    stream.write( mBody.str().c_str(), mBodySize );

    return stream.str();
}
