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
#include "../ExploreGame.h"
#include "../ItemFactory.h"
#include <engine/LoggerSingleton.h>
#include <engine/LuaBinder.h>
#include <iostream>
#include <boost/foreach.hpp>
#include "../Explore.h"
#include "../players/LocalPlayer.h"

using namespace boost::asio::ip;
using namespace boost::chrono;

enum E_STATUS_BITS
{
    ESB_SERVER = 0,
    ESB_WAIT_FOR_INFO,
    ESB_WAIT_FOR_CONNECTION,
    ESB_CONNECTED,
    ESB_COUNT
};

enum E_CLIENT_STATUS_BITS
{
    ECSB_INITIALIZED = 0,
    ECSB_PLAYERS_CREATED,
    ECSB_ITEMS_CREATED,
    ECSB_COUNT
};

ExploreServer::ExploreServer( ExplorePtr explore, const HostInfo &info,
                              NetworkMessengerPtr messenger )
    : NetworkSyncable( 1, 0 ),
      mExplore( explore ),
      mMessenger( messenger ),
      mStatusBits( ESB_COUNT ),
      mUpdateTimer( *mExplore->getIOService().get() )
{
    mSelfInfo.host = info;
    setUpdateInterval( 200 );
    update();
}

NetworkMessengerPtr ExploreServer::getNetworkMessenger() const
{
    return mMessenger;
}

void ExploreServer::setSelfInfo( const ExploreServer::HostInfo &info )
{
    mSelfInfo.host = info;
}

ExploreServer::HostInfo ExploreServer::selfInfo() const
{
    return mSelfInfo.host;
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

void ExploreServer::setUpdateInterval(const int &interval)
{
    mUpdateInterval = interval;
}

int ExploreServer::updateInterval() const
{
    return mUpdateInterval;
}

void ExploreServer::requestServerInfo( const std::string &ip, const int &port )
{
    _LOG( "Requesting server info from",
          ip + std::string( ":" ) + boost::lexical_cast<std::string>( port ) );
    mStatusBits[ESB_WAIT_FOR_INFO] = true;
    mMessenger->sendTo( serialize( ESAID_REQUEST_SERVERINFO ), ip, port );
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

    mMessenger->sendTo( serialize( ESAID_REQUEST_CONNECTION ), ip, port );
}

bool ExploreServer::isConnecting() const
{
    return mStatusBits[ESB_WAIT_FOR_CONNECTION];
}

bool ExploreServer::hasConnection() const
{
    return mStatusBits[ESB_CONNECTED];
}

void ExploreServer::setClientTimeout(int timeout)
{
    mClientTimeout = timeout;
}

int ExploreServer::clientTimeout() const
{
    return mClientTimeout;
}

uint32_t ExploreServer::clientID() const
{
    return mSelfInfo.id;
}

void ExploreServer::disconnect()
{
    mStatusBits[ESB_CONNECTED] = false;
}

void ExploreServer::update()
{
    //Create new items and distribute them if this is a server
    //TODO: Maybe refactor to WorldPlayer or somewhere else?
    while( mMessenger->hasPacketsInQueue() && mExplore->getGameState() == EGS_GAME )
    {
        NetworkSyncablePacket packet = mMessenger->nextPacket();

        if( packet.getTypeID() == ENTI_ITEM )
        {
            if( packet.getActionID() == EAID_CREATE )
                ItemFactory::create( mExplore, packet );
            else
            {
                NetworkSyncablePacket p = serialize( ESAID_REQUEST_ITEM_INFO );
                p.writeUInt32( packet.getUID() );
                send( p );
            }
        }
        else if( packet.getTypeID() == ENTI_PLAYER )
        {
            uint32_t clientID = packet.readUInt32();
            VisualPlayerPtr player;

            WorldPlayerPtr world =
                    mExplore->getExploreGame()->getWorldPlayer();

            if( clientID == mSelfInfo.id )
            {
                player.reset( new LocalPlayer( mExplore, world ) );
                world->setLocalPlayer( player );
            }
            else
                player.reset( new VisualPlayer( mExplore, world ) );
            player->setClientID( clientID );
            player->deserialize( packet );
        }

        if( mStatusBits[ESB_SERVER] )
            send( packet );
    }
    if( mStatusBits[ESB_SERVER] )
    {
        system_clock::time_point now = system_clock::now();
        system_clock::duration then = seconds( mClientTimeout );

        typedef std::map<uint32_t, ClientInfo> map_t;
        foreach_( map_t::value_type &x, mClientIDMap )
        {
            if( x.second.statusBits[ECSB_INITIALIZED]
                    && x.second.lastActiveTime + then <= now )
            {
                std::string name = x.second.host.hostName;
                _LOG( "Timeout; No response from", name );
                _LOG( "Kicking client", name );
                mClientIDMap.erase( x.second.id );
                break;
            }
            else if( !x.second.statusBits[ECSB_INITIALIZED] )
            {
                std::list<NetworkSyncablePacket> newList;
                mExplore->getExploreGame()->getWorldPlayer()->serializeAll(
                            EAID_CREATE, newList );

                foreach_( NetworkSyncablePacket &packet, newList )
                    mMessenger->sendTo( packet, x.second.endpoint );

                x.second.statusBits[ECSB_INITIALIZED] = true;
                //TODO:send more infos to be able to show a status bar
            }
        }

        send( serialize( ESAID_REQUEST_IS_STILL_ALIVE ) );

        if( mExplore->getExploreGame()->getWorldPlayer() )
        {//TODO:this does not look neat
            std::list<NetworkSyncablePacket> syncableList;
            mExplore->getExploreGame()->getWorldPlayer()->serializeAll(
                        EAID_UPDATE, syncableList );

            foreach_( NetworkSyncablePacket &packet, syncableList )
                send( packet );
        }
    }

    mUpdateTimer.expires_from_now(
                boost::posix_time::milliseconds( mUpdateInterval ) );
    mUpdateTimer.async_wait(
                boost::bind( &ExploreServer::update, this ) );
}

void ExploreServer::send( const NetworkSyncablePacket &packet )
{
    if( mStatusBits[ESB_SERVER] )
    {
        typedef std::map<uint32_t, ClientInfo> map_t;
        foreach_( map_t::value_type &x, mClientIDMap )
        {
            if( x.second.statusBits[ECSB_INITIALIZED] )
                mMessenger->sendTo( packet, x.second.endpoint );
        }
    }
    else
        mMessenger->send( packet );
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case ESAID_REQUEST_SERVERINFO:
    {
        if( mStatusBits[ESB_WAIT_FOR_INFO] )
        {
            _LOG( "Server info received" );
            HostInfo info;
            info.hostName = packet.readString();
            info.serverMaxPlayers = packet.readUInt8();
            info.serverConnectedPlayers = packet.readUInt8();
            mServerInfoQueue.push( info );
            mStatusBits[ESB_WAIT_FOR_INFO] = false;
        }
        else if( mStatusBits[ESB_SERVER] )
        {
            _LOG( "Client requesting server info" );
            return serialize( ESAID_REQUEST_SERVERINFO );
        }
        break;
    }
    case ESAID_REQUEST_CONNECTION:
    {
        if( mStatusBits[ESB_SERVER] )
        {
            _LOG( "Client requesting connection from ",
                  packet.getEndpoint().address().to_string() );
            HostInfo host;
            host.hostName = packet.readString();
            host.passwordHash = packet.readString();

            if( mSelfInfo.host.serverConnectedPlayers < mSelfInfo.host.serverMaxPlayers &&
                    host.passwordHash == mSelfInfo.host.passwordHash )
            {
                ClientInfo info;
                info.endpoint = packet.getEndpoint();
                info.id = nextClientID();
                info.host = host;
                info.lastActiveTime = system_clock::now();
                mClientIDMap.insert( std::make_pair( info.id, info ) );

                _LOG( "Client accepted!" );
                _LOG( "New client's ID", info.id );
                _LOG( "New client's name", info.host.hostName );

                return serialize( ESAID_ACCEPT_CONNECTION );
            }
            else
            {
                _LOG( "Connection not accepted" );
                return serialize( ESAID_NAK );
            }
        }
        else
        {
            _LOG( "Client requesting connection, but serverMode is false" );
            return serialize( ESAID_NAK );
        }
        break;
    }
    case ESAID_ACK:
    {
        if( mStatusBits[ESB_SERVER] ) //Receive is_alive
        {
            uint32_t clientID = packet.readUInt32();
            std::map<uint32_t,ClientInfo>::iterator x = mClientIDMap.find( clientID );
            if( x == mClientIDMap.end() )
                _LOG( "Unknown ACK received with ClientID", clientID );
            else
            {
                x->second.lastActiveTime = system_clock::now();
            }
        }
        else
            _LOG( "ACK received" );
        break;
    }
    case ESAID_NAK:
    {
        _LOG( "NAK received" );
        break;
    }
    case ESAID_ACCEPT_CONNECTION:
    {
        if( mStatusBits[ESB_WAIT_FOR_CONNECTION] && !mStatusBits[ESB_SERVER] )
        {
            _LOG( "Connection accepted!" );
            mStatusBits[ESB_CONNECTED] = true;
            mStatusBits[ESB_WAIT_FOR_CONNECTION] = false;

            boost::asio::ip::udp::endpoint endpoint = packet.getEndpoint();
            mMessenger->setRemoteAddress( endpoint.address().to_string(),
                                          endpoint.port() );

            mSelfInfo.id = packet.readUInt32();
            _LOG( "New ClientID", mSelfInfo.id );
        }
        break;
    }
    case ESAID_REQUEST_IS_STILL_ALIVE:
    {
        if( mStatusBits[ESB_CONNECTED] )
            return serialize( ESAID_ACK );
        break;
    }
    case ESAID_REQUEST_ITEM_INFO:
    {
        uint32_t uuid = packet.readUInt32();

        NetworkSyncable *syncable = NetworkSyncable::getObject( uuid );
        if( syncable )
            return syncable->serialize( EAID_CREATE );

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
    case ESAID_REQUEST_SERVERINFO:
    {
        packet.writeString( mSelfInfo.host.hostName );
        packet.writeUInt8( mSelfInfo.host.serverMaxPlayers );
        packet.writeUInt8( mSelfInfo.host.serverConnectedPlayers );
        break;
    }
    case ESAID_REQUEST_CONNECTION:
    {
        packet.writeString( mSelfInfo.host.hostName );
        packet.writeString( mSelfInfo.host.passwordHash );
        break;
    }
    case ESAID_ACCEPT_CONNECTION:
    {
        packet.writeUInt32( nextClientID() - 1 );
        break;
    }
    case ESAID_ACK:
    {
        if( !mStatusBits[ESB_SERVER] ) //Alive ACK
            packet.writeUInt32( mSelfInfo.id );
        break;
    }
    default:
    {
        break;
    }
    }
}

uint32_t ExploreServer::nextClientID()
{
    if( mClientIDMap.empty() )
        return 1;

    std::map<uint32_t,ClientInfo>::iterator x = mClientIDMap.end()--;
    return x->first + 1;
}

ExploreServer::ClientInfo::ClientInfo()
    : id( 0 ),
      statusBits( ECSB_COUNT )
{
}
