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

#ifndef NETWORKSYNCABLEPACKET_H
#define NETWORKSYNCABLEPACKET_H

#include <string>
#include <sstream>
#include <stdint.h>

class NetworkSyncablePacket
{
public:
    NetworkSyncablePacket( const std::string &data );
    NetworkSyncablePacket( uint32_t uid, uint8_t typeID, uint8_t actionID,
                           const std::string &body );
    NetworkSyncablePacket( const NetworkSyncablePacket &other );

    uint32_t getUID() const;
    uint8_t getTypeID() const;
    uint8_t getActionID() const;
    uint32_t getBodySize() const;
    std::string getBody() const;

    bool isValid();

    void writeUInt8( const uint8_t &val );
    void writeUInt16( const uint16_t &val );
    void writeUInt32( const uint32_t &val );
    void writeUInt64( const uint64_t &val );
    void writeInt8( const int8_t &val );
    void writeInt16( const int16_t &val );
    void writeInt32( const int32_t &val );
    void writeInt64( const int64_t &val );
    void writeFloat( const float &val );
    void writeString( const std::string &val );
    void writeBool( const bool &val );

    uint8_t readUInt8();
    uint16_t readUInt16();
    uint32_t readUInt32();
    uint64_t readUInt64();
    int8_t readInt8();
    int16_t readInt16();
    int32_t readInt32();
    int64_t readInt64();
    float readFloat();
    std::string readString();
    bool readBool();

    std::string serialize() const;

private:
    uint32_t mUID;
    uint8_t mTypeID, mActionID;
    uint32_t mBodySize;
    std::stringstream mBody;

    bool mValid;
};

#endif // NETWORKSYNCABLEPACKET_H
