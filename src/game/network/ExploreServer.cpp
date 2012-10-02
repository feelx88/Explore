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

#include "ExploreServer.h"
#include <iostream>

enum E_ACTIONID
{
    EAID_ACK = 0,
    EAID_NAK,
    EAID_REQUEST_SERVERINFO,
    EAID_SEND_SERVERINFO,
    EAID_CONNECT
};

enum E_STATUS_BITS
{
    ESB_RUNNING = 1
};

struct LoginRequest
{
    std::string playerName;
    std::string passwordHash;
};

ExploreServer::ExploreServer( const ServerInfo &info )
    : mSelfInfo( info )
{
}

void ExploreServer::requestServerInfo( NetworkMessenger *msg,
                                       const std::string &ip, const int &port )
{
    msg->sendTo( serialize( EAID_REQUEST_SERVERINFO ), ip, port );
}

bool ExploreServer::hasServerInfo() const
{
    return !mServerInfoQueue.empty();
}

ExploreServer::ServerInfo ExploreServer::nextServerInfo()
{
    ServerInfo info = mServerInfoQueue.front();
    mServerInfoQueue.pop();
    return info;
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternal( NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case EAID_REQUEST_SERVERINFO:
    {
        return serialize( EAID_SEND_SERVERINFO );
        break;
    }
    case EAID_SEND_SERVERINFO:
    {
        ServerInfo info;
        info.ServerName = packet.readString();
        info.maxPlayers = packet.readUInt8();
        info.connectedPlayers = packet.readUInt8();
        mServerInfoQueue.push( info );
        break;
    }
    default:
    {
        break;
    }
    }
    return boost::none;
}

void ExploreServer::serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID )
{
    switch( actionID )
    {
    case EAID_REQUEST_SERVERINFO:
    {
        break;
    }
    case EAID_SEND_SERVERINFO:
    {
        packet.writeString( mSelfInfo.ServerName );
        packet.writeUInt8( mSelfInfo.maxPlayers );
        packet.writeUInt8( mSelfInfo.connectedPlayers );
        break;
    }
    default:
    {
        break;
    }
    }
}
