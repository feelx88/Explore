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
#include <engine/LoggerSingleton.h>
#include <engine/LuaBinder.h>
#include <iostream>
#include "../Explore.h"

using namespace boost::asio::ip;

enum E_ACTIONID
{
    EAID_ACK = 0,
    EAID_NAK,
    EAID_REQUEST_SERVERINFO,
    EAID_REQUEST_CONNECTION,
    EAID_REQUEST_IS_STILL_ALIVE
};

enum E_STATUS_BITS
{
    ESB_SERVER = 0,
    ESB_WAIT_FOR_INFO,
    ESB_WAIT_FOR_CONNECTION,
    ESB_CONNECTED,
    ESB_COUNT
};

class ExploreServerBinder : public LuaBinder
{
public:
    void reg( LuaStatePtr state )
    {
        using namespace luabind;

        module( state.get() )
        [
            class_<ExploreServer>( "ExploreServer" )
                .def( "setServerMode", &ExploreServer::setServerMode )
                .def( "requestServerInfo", &ExploreServer::requestServerInfo )
                .def( "hasServerInfo", &ExploreServer::hasServerInfo )
                .def( "nextServerInfo", &ExploreServer::nextServerInfo )
                .def( "requestConnection", &ExploreServer::requestConnection )
                .def( "getNetworkMessenger", &ExploreServer::getNetworkMessenger )
                .def( "serialize", &ExploreServer::serialize )
                .enum_( "E_ACTIONID" )
                [
                    value( "EAID_ACK", EAID_ACK ),
                    value( "EAID_NAK", EAID_NAK ),
                    value( "EAID_REQUEST_SERVERINFO", EAID_REQUEST_SERVERINFO ),
                    value( "EAID_REQUEST_CONNECTION", EAID_REQUEST_CONNECTION ),
                    value( "EAID_REQUEST_IS_STILL_ALIVE", EAID_REQUEST_IS_STILL_ALIVE )
                ]
                .scope
                [
                    class_<ExploreServer::HostInfo>( "HostInfo" )
                        .def( constructor<>() )
                        .def_readwrite( "hostName",
                                        &ExploreServer::HostInfo::hostName )
                ]
        ];
    }

private:
    static int regDummy;
};
int ExploreServerBinder::regDummy = LuaBinder::registerBinder( new ExploreServerBinder );

ExploreServer::ExploreServer( ExplorePtr explore, const HostInfo &info,
                              NetworkMessengerPtr messenger )
    : NetworkSyncable( 0, 0 ),
      mExplore( explore ),
      mMessenger( messenger ),
      mStatusBits( ESB_COUNT ),
      mSelfInfo( info )
{
}

NetworkMessengerPtr ExploreServer::getNetworkMessenger() const
{
    return mMessenger;
}

void ExploreServer::setSelfInfo( const ExploreServer::HostInfo &info )
{
    mSelfInfo = info;
}

ExploreServer::HostInfo ExploreServer::selfInfo() const
{
    return mSelfInfo;
}

void ExploreServer::setServerMode( bool server )
{
    _LOG( "Server mode set to", server );
    mStatusBits[ESB_SERVER] = server;
}

bool ExploreServer::serverMode() const
{
    return mStatusBits[ESB_SERVER];
}

void ExploreServer::requestServerInfo( const std::string &ip, const int &port )
{
    _LOG( "Requesting server info from",
          ip + std::string( ":" ) + boost::lexical_cast<std::string>( port ) );
    mStatusBits[ESB_WAIT_FOR_INFO] = true;
    mMessenger->sendTo( serialize( EAID_REQUEST_SERVERINFO ), ip, port );
}

bool ExploreServer::hasServerInfo() const
{
    bool status = !mServerInfoQueue.empty();
    _LOG( "ExploreServer::hasServerInfo", status );
    return status;
}

ExploreServer::HostInfo ExploreServer::nextServerInfo()
{
    HostInfo info = mServerInfoQueue.front();
    mServerInfoQueue.pop();
    return info;
}

void ExploreServer::requestConnection( const std::string &ip, const int &port )
{
    _LOG( "Requesting connection to",
          ip + std::string( ":" ) + boost::lexical_cast<std::string>( port ) );

    mStatusBits[ESB_WAIT_FOR_CONNECTION] = true;
    mStatusBits[ESB_CONNECTED] = false;

    mMessenger->sendTo( serialize( EAID_REQUEST_CONNECTION ), ip, port );
}

bool ExploreServer::isConnection() const
{
    return mStatusBits[ESB_WAIT_FOR_CONNECTION];
}

bool ExploreServer::hasConnection() const
{
    return mStatusBits[ESB_CONNECTED];
}

void ExploreServer::updateConnectedClients()
{
    for( std::vector<ClientInfo>::iterator x = mClientInfo.begin();
         x != mClientInfo.end(); ++x )
    {
        //if not alive -> kick
        mMessenger->sendTo( serialize( EAID_REQUEST_IS_STILL_ALIVE ), x->endpoint );
        //TODO:static NetworkSyncable->update
    }
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case EAID_REQUEST_SERVERINFO:
    {
        _LOG( "SERVERINFO_REQUEST received" );
        if( mStatusBits[ESB_WAIT_FOR_INFO] )
        {
            HostInfo info;
            info.hostName = packet.readString();
            info.serverMaxPlayers = packet.readUInt8();
            info.serverConnectedPlayers = packet.readUInt8();
            mServerInfoQueue.push( info );
            mStatusBits[ESB_WAIT_FOR_INFO] = false;
        }
        else
            return serialize( EAID_REQUEST_SERVERINFO );
        break;
    }
    case EAID_REQUEST_CONNECTION:
    {
        _LOG( "CONNECTION_REQUEST received" );
        if( mStatusBits[ESB_SERVER] )
        {
            HostInfo host;
            host.hostName = packet.readString();
            host.passwordHash = packet.readString();

            if( mSelfInfo.serverConnectedPlayers < mSelfInfo.serverMaxPlayers &&
                    host.passwordHash == mSelfInfo.passwordHash )
            {
                ClientInfo info;
                info.endpoint = packet.getEndpoint();
                mClientInfo.push_back( info );
                return serialize( EAID_ACK );
            }
            else
                return serialize( EAID_NAK );
        }
        break;
    }
    case EAID_ACK:
    {
        if( mStatusBits[ESB_WAIT_FOR_CONNECTION] )
        {
            _LOG( "Connection accepted!" );
            mStatusBits[ESB_CONNECTED] = true;

            boost::asio::ip::udp::endpoint endpoint = packet.getEndpoint();
            mMessenger->setRemoteAddress( endpoint.address().to_string(),
                                          endpoint.port() );
        }
        else if( mStatusBits[ESB_SERVER] ) //Receive is_alive
        {
            ClientInfo *info = mEndpointClientMap[packet.getEndpoint().address().to_string()];
            if( !info )
                _LOG( "Unknown ACK received", packet.getEndpoint().address() );
        }
        else
            _LOG( "ACK received" );
        break;
    }
    case EAID_REQUEST_IS_STILL_ALIVE:
    {
        return serialize( EAID_ACK );
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
        packet.writeString( mSelfInfo.hostName );
        packet.writeUInt8( mSelfInfo.serverMaxPlayers );
        packet.writeUInt8( mSelfInfo.serverConnectedPlayers );
        break;
    }
    case EAID_REQUEST_CONNECTION:
    {
        packet.writeString( mSelfInfo.hostName );
        packet.writeString( mSelfInfo.passwordHash );
        break;
    }
    case EAID_ACK:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}
