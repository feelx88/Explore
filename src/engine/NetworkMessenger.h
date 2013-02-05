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

class APIEXPORT NetworkMessenger
{
public:
    NetworkMessenger( IOServicePtr ioService, PropTreePtr properties );
    virtual ~NetworkMessenger();

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

    IOServicePtr mIOService;
    PropTreePtr mProperties;

    UDPSocketPtr mSocket;

    std::string mServerIP;
    int mPort;

    std::vector<unsigned char> mReceiveBuffer;
    std::queue<NetworkSyncablePacket> mPacketQueue;
    UDPEndpoint mRemoteEndpoint;

    boost::asio::deadline_timer mCheckedSendTimer;
    int mCheckedSendTimerTimeout;
    boost::unordered::unordered_multimap<uint32_t,
        std::pair<NetworkSyncablePacket, UDPEndpoint> > mCheckedSendPackets;
};

#endif //NETWORKMESSENGER_H
