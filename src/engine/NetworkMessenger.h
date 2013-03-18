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

#ifndef NETWORKMESSENGER_H
#define NETWORKMESSENGER_H

#include "EngineTypedefs.h"
#include "NetworkSyncablePacket.h"
#include "NetworkSyncable.h"
#include <queue>
#include <boost/unordered_map.hpp>

class NetworkMessenger;

typedef boost::shared_ptr<NetworkMessenger> NetworkMessengerPtr;
typedef boost::asio::ip::udp::endpoint UDPEndpoint;
typedef std::queue<NetworkSyncablePacket> PacketQueue;

class APIEXPORT NetworkMessenger
{
public:

    struct APIEXPORT Connection
    {
        Connection();

        UDPEndpoint endpoint;
        uint8_t id, foreignID, sequenceCounter;
        bool connected;
    };

    typedef std::map<uint8_t, Connection> ConnectionMap;

    NetworkMessenger( IOServicePtr ioService, PropTreePtr properties );
    virtual ~NetworkMessenger();

    boost::optional<Connection> connect( std::string host, int port );

    void send( const NetworkSyncablePacket &packet );
    void sendTo( const NetworkSyncablePacket &packet,
                 const UDPEndpoint &endpoint );
    void sendTo( const NetworkSyncablePacket &packet, const std::string &ip,
                 const int &port );

    void checkedSend( NetworkSyncablePacket &packet );
    void checkedSendTo( NetworkSyncablePacket &packet,
                 const UDPEndpoint &endpoint );
    void checkedSendTo( NetworkSyncablePacket &packet, const std::string &ip,
                 const int &port );

    void setCheckedSendTimeout( int timeoutMS );
    int checkedSendTimeout() const;

    bool hasPacketsInQueue() const;
    NetworkSyncablePacket nextPacket( bool pop = true );
    void reQueuePacket();
    void popPacket();

    void setRemoteAddress( const std::string &address, const int &port );
    void bind( int port );

private:
    void receive();

    void receiveHandler( const boost::system::error_code &error, size_t );
    void sendHandler( const boost::system::error_code &error, size_t );

    void checkedSendHandler();

    uint8_t getNextConnectionID() const;

    IOServicePtr mIOService;
    PropTreePtr mProperties;

    UDPSocketPtr mSocket;

    std::string mServerIP;
    int mPort;

    std::vector<unsigned char> mReceiveBuffer;
    PacketQueue mPacketQueue;
    UDPEndpoint mRemoteEndpoint;

    boost::asio::deadline_timer mCheckedSendTimer;
    int mCheckedSendTimerTimeout;
    boost::unordered::unordered_multimap<uint32_t,
        std::pair<NetworkSyncablePacket, UDPEndpoint> > mCheckedSendPackets;

    PacketQueue mCheckedSendQueue;

    ConnectionMap mConnections;
    std::map<uint8_t, uint8_t> mForeignConnectionIDs;
    std::vector<uint8_t> mWaitingConnections;

};

#endif //NETWORKMESSENGER_H
