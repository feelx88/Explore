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

#include "EngineTypedefs.h"
#include <string>
#include <sstream>
#include <stdint.h>
#include <boost/asio.hpp>

/*
 * General packet structure [Version 3]:
 *
 *------------------------------*
 *  Packet type: char           *
 *------------------------------*
 *  Connection id: uint8_t      *
 *------------------------------*
 *  Sequence counter: uint8_t   *
 *------------------------------*
 *  UID: uint32_t               *
 *------------------------------*
 *  ActionID: uint8_t           *
 *------------------------------*
 *  TypeID: uint8_t             *
 *------------------------------*
 *  [Body]                      *
 *------------------------------*
 *
 * Connection id is always the connection id of the other side of the connection!
 * If there is a body, it consists of:
 *
 *------------------------------*
 *  Body length: uint32_t       *
 *------------------------------*
 *  Body data                   *
 *------------------------------*
 *
 * Packet types:
 * - 'n': Normal, unchecked data packet
 *   + Connection id can be 0
 *   + Sequence counter is 0
 *   + Body: data
 * - 'c': Checked data packet
 *   + Body: data
 * - 'p': Pingback packet
 *   + No body
 * - 'i': Connection initialization packet
 *   + No body
 *   + UID, ActionID and TypeID are 0
 * - 'r': Connection initialization response packet
 *   + UID, ActionID and TypeID are 0
 *   + Body: uint8_t with foreign connection id
 *
 */

class APIEXPORT NetworkSyncablePacket
{
public:
    enum E_PACKET_TYPE
    {
        EPT_NORMAL = 0,
        EPT_CHECKED,
        EPT_PINGBACK,
        EPT_INITIALIZATION,
        EPT_INITIALIZATION_RESPONSE
    };

    NetworkSyncablePacket( const std::string &data );
    NetworkSyncablePacket( uint32_t uid, uint8_t typeID, uint8_t actionID,
                           const std::string &body );
    NetworkSyncablePacket( const NetworkSyncablePacket &other );

    void swap( NetworkSyncablePacket &other );
    NetworkSyncablePacket& operator=( NetworkSyncablePacket other );

    uint32_t getUID() const;
    uint8_t getConnectionID() const;
    uint8_t getSequenceCounter() const;
    uint8_t getTypeID() const;
    uint8_t getActionID() const;
    uint32_t getBodySize() const;
    std::string getBody() const;

    void clearBody();

    bool isValid();

    void setPacketType( E_PACKET_TYPE mode );
    E_PACKET_TYPE getPacketType() const;

    void setConnectionID( uint8_t id );
    void setSequenceCounter( uint8_t seq );

    boost::asio::ip::udp::endpoint getEndpoint();
    void setEndpoint( const boost::asio::ip::udp::endpoint &endpoint );

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
    inline char packetTypeToChar( E_PACKET_TYPE mode ) const;
    inline E_PACKET_TYPE charToPacketType( char mode ) const;

    uint32_t mUID;
    uint8_t mConnectionID, mSequenceCounter, mTypeID, mActionID;
    uint32_t mBodySize;
    std::stringstream mBody;

    bool mValid;
    char mPacketType;

    boost::asio::ip::udp::endpoint mEndpoint;
};

namespace std
{
    template <>
    APIEXPORT inline void swap<NetworkSyncablePacket>( NetworkSyncablePacket &left,
                                      NetworkSyncablePacket &right )
    {
        left.swap( right );
    }
}

#endif // NETWORKSYNCABLEPACKET_H
