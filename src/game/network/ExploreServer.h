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

#ifndef EXPLORESERVER_H
#define EXPLORESERVER_H

#include <engine/NetworkSyncable.h>
#include <engine/NetworkMessenger.h>
#include <boost/dynamic_bitset.hpp>
#include <boost/chrono.hpp>
#include "../GameState.h"

class APIEXPORT ExploreServer : public NetworkSyncable
{
public:
    enum E_SERVER_ACTIONID
    {
        ESAID_ACK = 0,
        ESAID_NAK,
        ESAID_REQUEST_SERVERINFO,
        ESAID_REQUEST_CONNECTION,
        ESAID_ACCEPT_CONNECTION,
        ESAID_REQUEST_IS_STILL_ALIVE,
        ESAID_REQUEST_ITEM_INFO,
        ESAID_CONNECTIONINFO
    };

    struct InitializationInfo
    {
        size_t curPlayers, totalPlayers;
        size_t curItems, totalItems;
    };

    struct HostInfo
    {
        std::string hostName;
        uint8_t serverMaxPlayers, serverConnectedPlayers;
        std::string passwordHash;
    };

    struct ClientInfo
    {
        ClientInfo();

        uint32_t id;
        HostInfo host;
        boost::asio::ip::udp::endpoint endpoint;
        boost::chrono::system_clock::time_point lastActiveTime;
        boost::dynamic_bitset<> statusBits;
        InitializationInfo initializationInfo;
    };

    ExploreServer( ExplorePtr explore, const HostInfo &info,
                   NetworkMessengerPtr messenger );

    NetworkMessengerPtr getNetworkMessenger() const;

    void setSelfInfo( const HostInfo &info );
    HostInfo selfInfo() const;

    void setServerMode( bool server );
    bool serverMode() const;
    void setUpdateInterval( const int &interval );
    int updateInterval() const;

    void requestServerInfo( const std::string &ip, const int &port );
    bool hasServerInfo() const;
    HostInfo nextServerInfo();

    void requestConnection( const std::string &ip, const int &port );
    bool isConnecting() const;
    bool hasConnection() const;
    bool isInitialized() const;

    void setClientTimeout( int timeout );
    int clientTimeout() const;

    uint32_t clientID() const;

    void disconnect();

    void update();

    void send( const NetworkSyncablePacket &packet );
    void checkedSend( const NetworkSyncablePacket &packet );

protected:
    boost::optional<NetworkSyncablePacket> deserializeInternal(
            NetworkSyncablePacket &packet );
    void serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID );

    boost::optional<NetworkSyncablePacket> deserializeInternalServer(
            NetworkSyncablePacket &packet );
    boost::optional<NetworkSyncablePacket> deserializeInternalClient(
        NetworkSyncablePacket &packet );

    void serializeInternalServer( NetworkSyncablePacket &packet, uint8_t actionID );
    void serializeInternalClient( NetworkSyncablePacket &packet, uint8_t actionID );

    uint32_t nextClientID();

    void handleInitPackets();

    ExplorePtr mExplore;

    NetworkMessengerPtr mMessenger;

    boost::dynamic_bitset<> mStatusBits;

    std::queue<HostInfo> mServerInfoQueue;
    ClientInfo mSelfInfo;

    std::map<uint32_t, ClientInfo> mClientIDMap;

    boost::asio::deadline_timer mUpdateTimer;
    int mUpdateInterval;
    int mClientTimeout;
};

#endif // EXPLORESERVER_H
