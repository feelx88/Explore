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
#include "LuaBinder.h"
#include "LoggerSingleton.h"
#include <sstream>
#include <cmath>
#include <cstring>

#define MANTISSA_SIZE 23  //ieee 754
#define INT_SIZE 4
#define PACKED_FLOAT_SIZE 8

NetworkSyncablePacket::NetworkSyncablePacket( const std::string &data )
{
    std::stringstream stream;
    stream.write( data.data(), data.size() );

    stream.read( reinterpret_cast<char*>( &mUID ), sizeof( uint32_t ) );
    stream.read( reinterpret_cast<char*>( &mActionID ), sizeof( uint8_t ) );
    stream.read( reinterpret_cast<char*>( &mTypeID ), sizeof( uint8_t ) );
    stream.read( reinterpret_cast<char*>( &mBodySize ), sizeof( uint32_t ) );

    char *tmp = new char[mBodySize];
    stream.read( tmp, mBodySize );

    mBody.write( tmp, mBodySize );
    delete[] tmp;
}

NetworkSyncablePacket::NetworkSyncablePacket( uint32_t uid, uint8_t typeID,
                                              uint8_t actionID,
                                              const std::string &body )
    : mUID( uid ),
      mTypeID( typeID ),
      mActionID( actionID ),
      mBodySize( body.length() )
{
    mBody.write( body.data(), mBodySize );
}

NetworkSyncablePacket::NetworkSyncablePacket( const NetworkSyncablePacket &other )
    : mUID( other.getUID() ),
      mTypeID( other.getTypeID() ),
      mActionID( other.getActionID() ),
      mBodySize( other.getBodySize() )
{
    mBody.write( other.getBody().data(), other.getBodySize() );
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

boost::asio::ip::udp::endpoint NetworkSyncablePacket::getEndpoint()
{
    return mEndpoint;
}

void NetworkSyncablePacket::setEndpoint( const boost::asio::ip::udp::endpoint &endpoint )
{
    mEndpoint = endpoint;
}

void NetworkSyncablePacket::writeUInt8( const uint8_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( uint8_t ) );
    mBodySize += sizeof( uint8_t );
}

void NetworkSyncablePacket::writeUInt16( const uint16_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( uint16_t ) );
    mBodySize += sizeof( uint16_t );
}

void NetworkSyncablePacket::writeUInt32( const uint32_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( uint32_t ) );
    mBodySize += sizeof( uint32_t );
}

void NetworkSyncablePacket::writeUInt64( const uint64_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( uint64_t ) );
    mBodySize += sizeof( uint64_t );
}

void NetworkSyncablePacket::writeInt8( const int8_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( int8_t ) );
    mBodySize += sizeof( int8_t );
}

void NetworkSyncablePacket::writeInt16( const int16_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( int16_t ) );
    mBodySize += sizeof( int16_t );
}

void NetworkSyncablePacket::writeInt32( const int32_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( int32_t ) );
    mBodySize += sizeof( int32_t );
}

void NetworkSyncablePacket::writeInt64( const int64_t &val )
{
    mBody.write( reinterpret_cast<const char*>( &val ), sizeof( int64_t ) );
    mBodySize += sizeof( int64_t );
}

void NetworkSyncablePacket::writeFloat( const float &val )
{
    int32_t exponent = 0;
    float mantissa = frexp( val, &exponent );
    int32_t mantissaInt = ldexp( mantissa, MANTISSA_SIZE );

    writeInt32( exponent );
    writeInt32( mantissaInt );
}

void NetworkSyncablePacket::writeString( const std::string &val )
{
    uint32_t size = val.length();
    writeUInt32( size );
    mBody.write( val.c_str(), size );
    mBodySize += size;
}

void NetworkSyncablePacket::writeBool( const bool &val )
{
    char x = val ? '1' : '0';
    mBody.write( &x, 1 );
    mBodySize += 1;
}

uint8_t NetworkSyncablePacket::readUInt8()
{
    uint8_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( uint8_t ) );
    mBodySize -= sizeof( uint8_t );
    return val;
}

uint16_t NetworkSyncablePacket::readUInt16()
{
    uint16_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( uint16_t ) );
    mBodySize -= sizeof( uint16_t );
    return val;
}

uint32_t NetworkSyncablePacket::readUInt32()
{
    uint32_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( uint32_t ) );
    mBodySize -= sizeof( uint32_t );
    return val;
}

uint64_t NetworkSyncablePacket::readUInt64()
{
    uint64_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( uint64_t ) );
    mBodySize -= sizeof( uint64_t );
    return val;
}

int8_t NetworkSyncablePacket::readInt8()
{
    int8_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( int8_t ) );
    mBodySize -= sizeof( int8_t );
    return val;
}

int16_t NetworkSyncablePacket::readInt16()
{
    int16_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( int16_t ) );
    mBodySize -= sizeof( int16_t );
    return val;
}

int32_t NetworkSyncablePacket::readInt32()
{
    int32_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( int32_t ) );
    mBodySize -= sizeof( int32_t );
    return val;
}

int64_t NetworkSyncablePacket::readInt64()
{
    int64_t val = 0;
    mBody.read( reinterpret_cast<char*>( &val ), sizeof( int64_t ) );
    mBodySize -= sizeof( int64_t );
    return val;
}

float NetworkSyncablePacket::readFloat()
{
    int32_t exponent = readInt32();
    int32_t mantissaInt = readInt32();

    float mantissa = (float)( mantissaInt ) / pow( 2.f, MANTISSA_SIZE );

    float v = ldexp( mantissa, exponent );

    return v;
}

std::string NetworkSyncablePacket::readString()
{
    uint32_t size = readUInt32();

    char* buf = new char[size];

    mBody.read( &buf[0], size );

    mBodySize -= size;

    std::string val = std::string( buf, size );

    delete[] buf;

    return val;
}

bool NetworkSyncablePacket::readBool()
{
    char x = '0';
    mBody.read( &x, 1 );
    mBodySize -= 1;
    return x == '1' ? true : false;
}

std::string NetworkSyncablePacket::serialize() const
{
    std::stringstream stream;

    std::string body = mBody.str();
    uint32_t size = body.length();

    stream.write( reinterpret_cast<const char*>( &mUID ), sizeof( uint32_t ) );
    stream.write( reinterpret_cast<const char*>( &mActionID ), sizeof( uint8_t ) );
    stream.write( reinterpret_cast<const char*>( &mTypeID ), sizeof( uint8_t ) );
    stream.write( reinterpret_cast<const char*>( &size ), sizeof( uint32_t ) );

    stream.write( body.data(), size );

    return stream.str();
}
