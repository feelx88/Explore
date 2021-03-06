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

#include <network/ExploreServer.h>
#include <ExploreGame.h>
#include <ItemFactory.h>
#include <engine/LoggerSingleton.h>
#include <engine/PythonTools.h>
#include <iostream>
#include <boost/foreach.hpp>
#include <Explore.h>
#include <players/LocalPlayer.h>

using namespace boost::asio::ip;
using namespace boost::chrono;

enum E_STATUS_BITS
{
    ESB_SERVER = 0,
    ESB_WAIT_FOR_INFO,
    ESB_WAIT_FOR_CONNECTION_PHASE1,
    ESB_WAIT_FOR_CONNECTION_PHASE2,
    ESB_CONNECTED,
    ESB_COUNT
};

enum E_CLIENT_STATUS_BITS
{
    ECSB_INITIALIZED = 0,
    ECSB_PACKETS_SENDED,
    ECSB_PLAYERS_CREATED,
    ECSB_ITEMS_CREATED,
    ECSB_WAIT_FOR_INITIAL_INFO_PACKET,
    ECSB_COUNT
};

ExploreServer::ExploreServer( ExplorePtr explore, const HostInfo &info,
                              NetworkMessengerPtr messenger )
    : NetworkSyncable( 1, 0 ),
      mExplore( explore ),
      mMessenger( messenger ),
      mStatusBits( ESB_COUNT ),
      mUpdateTimer( *mExplore->getIOService().get() ),
      mClientTimeout( 10 )
{
    mSelfInfo.host = info;
    setUpdateInterval( 50 );
    update();
    mStatusBits.reset();
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
    mMessenger->sendTo( serialize( EEAID_SERVERINFO_REQUEST ), ip, port );
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
    boost::optional<NetworkMessenger::ConnectionPtr> connection =
            mMessenger->connect( ip, port );

    if( connection )
    {
        _LOG( "Requesting connection to",
              ip + std::string( ":" ) + boost::lexical_cast<std::string>( port ) );

        mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE1] = true;
        mStatusBits[ESB_CONNECTED] = false;

        mSelfInfo.connection = *connection;
    }
    else
    {
        _LOG( "Host not found", ip );
    }

}

bool ExploreServer::isConnecting() const
{
    return mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE1] ||
            mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE2];
}

bool ExploreServer::hasConnection() const
{
    return mStatusBits[ESB_CONNECTED];
}

bool ExploreServer::isInitialized() const
{
    return mSelfInfo.statusBits[ECSB_INITIALIZED];
}

float ExploreServer::getConnectionProgress() const
{
    if( !mStatusBits[ESB_CONNECTED] )
    {
        return 0.f;
    }

    int total = mSelfInfo.initializationInfo.totalItems
            + mSelfInfo.initializationInfo.totalPlayers;
    int current = mSelfInfo.initializationInfo.curItems
            +mSelfInfo.initializationInfo.curPlayers;

    return (float)current / (float)total;
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
    checkedSend( serialize( EEAID_CONNECTION_QUIT ) );
    mStatusBits.reset();
    mSelfInfo.statusBits.reset();
}

void ExploreServer::update()
{
    handleInitPackets();

    if( mStatusBits[ESB_SERVER] )
    {
        WorldPlayerPtr world = mExplore->getExploreGame()->getWorldPlayer();

        system_clock::time_point now = system_clock::now();
        system_clock::duration then = seconds( mClientTimeout );

        typedef std::map<uint32_t, ClientInfo> map_t;
        foreach_( map_t::value_type &x, mClientIDMap )
        {
            //If client is inactive to long, kick him
            if( x.second.statusBits[ECSB_INITIALIZED] &&
                    now - x.second.lastActiveTime >= then )
            {
                std::string name = x.second.host.hostName;
                _LOG( "Timeout; No response from", name );
                _LOG( "Kicking client", name );

                removeClient( x.second );
                break;
            }

            if( !x.second.statusBits[ECSB_PACKETS_SENDED] )
            {
                //Send info packet
                NetworkSyncablePacket infoPacket =
                        serialize( EEAID_CONNECTIONINFO_SEND );

                std::list<NetworkSyncablePacket> playerList, itemList;
                world->serializeAll( EAID_CREATE, playerList, itemList );

                infoPacket.writeUInt8( playerList.size() );
                infoPacket.writeUInt32( itemList.size() );
                mMessenger->checkedSendTo( infoPacket, x.second.connection );

                foreach_( NetworkSyncablePacket &packet, playerList )
                {
                    mMessenger->checkedSendTo( packet, x.second.connection );
                }

                foreach_( NetworkSyncablePacket &packet, itemList )
                {
                    mMessenger->checkedSendTo( packet, x.second.connection );
                }

                x.second.statusBits[ECSB_PACKETS_SENDED] = true;
            }

            //Send request alive packet
            mMessenger->sendTo( serialize( EEAID_ALIVE_REQUEST ),
                                x.second.connection );
        }

        if( world )
        {//TODO:this does not look neat
            std::list<NetworkSyncablePacket> playerList, itemList;
            world->serializeAll( EAID_UPDATE, playerList, itemList );

            foreach_( NetworkSyncablePacket &packet, playerList )
                send( packet );
            foreach_( NetworkSyncablePacket &packet, itemList )
                send( packet );
        }
    }
    else if( mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE1] )
    {
        //If NetworkMessenger's connection is established, send Explore's
        //connection request
        if( mSelfInfo.connection->connected )
        {
            mMessenger->checkedSendTo( serialize( EEAID_CONNECTION_REQUEST ), mSelfInfo.connection );
            mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE1] = false;
            mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE2] = true;
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
                mMessenger->sendTo( packet, x.second.connection );
        }
    }
    else
        mMessenger->send( packet );
}

void ExploreServer::checkedSend( const NetworkSyncablePacket &packet )
{
    NetworkSyncablePacket copy( packet );
    copy.setPacketType( NetworkSyncablePacket::EPT_CHECKED );
    send( copy );
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternal(
        NetworkSyncablePacket &packet )
{
    if( mStatusBits[ESB_SERVER] )
        return deserializeInternalServer( packet );
    else
        return deserializeInternalClient( packet );
}

void ExploreServer::serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID )
{
    if( mStatusBits[ESB_SERVER] )
        serializeInternalServer( packet, actionID );
    else
        serializeInternalClient( packet, actionID );
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternalServer(
        NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case EEAID_SERVERINFO_REQUEST:
    {
        _LOG( "Client requesting server info" );
        return serialize( EEAID_SERVERINFO_RESPOND );
        break;
    }
    case EEAID_CONNECTION_REQUEST:
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
            info.connection = *mMessenger->getConnection( packet.getConnectionID() );
            info.id = nextClientID();
            info.host = host;
            info.lastActiveTime = system_clock::now();

            _LOG( "Client accepted!" );
            _LOG( "New client's ID", info.id );
            _LOG( "New client's name", info.host.hostName );

            mClientIDMap.insert( std::make_pair( info.id, info ) );

            NetworkSyncablePacket reply = serialize( EEAID_CONNECTION_RESPOND, true );
            reply.writeBool( true );

            return reply;
        }
        else
        {
            _LOG( "Connection not accepted" );
            NetworkSyncablePacket reply = serialize( EEAID_CONNECTION_RESPOND, true );
            reply.writeBool( false );

            return reply;
        }
        break;
    }
    case EEAID_CONNECTION_QUIT:
    {
        uint32_t clientID = packet.readUInt32();
        uint8_t reason = packet.readUInt8();
        _LOG( "Client disconnected", clientID );
        _LOG( "Reason", (int)reason ); //TODO: Reason code
        removeClient( mClientIDMap[clientID] );
        break;
    }
    case EEAID_ALIVE_RESPOND:
    {
        //Receive is_alive
        uint32_t clientID = packet.readUInt32();
        std::map<uint32_t,ClientInfo>::iterator x = mClientIDMap.find( clientID );
        if( x == mClientIDMap.end() )
            _LOG( "Unknown ALIVE_RESPOND received with ClientID", clientID );
        else
        {
            x->second.lastActiveTime = system_clock::now();
        }
        break;
    }
    case EEAID_ITEMINFO_REQUEST:
    {
        uint32_t uuid = packet.readUInt32();

        NetworkSyncable *syncable = NetworkSyncable::getObject( uuid );
        if( syncable )
            return syncable->serialize( EAID_CREATE );

        break;
    }
    case EEAID_INITIALIZATION_FINISH:
    {
        uint32_t id = packet.readUInt32();
        std::map<uint32_t,ClientInfo>::iterator x = mClientIDMap.find( id );
        if( x != mClientIDMap.end()
                && !x->second.statusBits[ECSB_INITIALIZED] )
        {
            x->second.statusBits[ECSB_INITIALIZED] = true;

            //Create player
            WorldPlayerPtr world = mExplore->getExploreGame()->getWorldPlayer();
            VisualPlayerPtr p( new VisualPlayer( mExplore, world ),
                               specialDeleters::NullDeleter() );
            p->setClientID( id );

            x->second.playerUID = p->getUID();
        }

        break;
    }
    default:
    {
        break;
    }
    }
    return boost::none;
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternalClient(
        NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case EEAID_SERVERINFO_RESPOND:
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
        break;
    }
    case EEAID_CONNECTION_RESPOND:
    {
        if( mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE2] )
        {
            uint32_t id = packet.readUInt32();
            bool accepted = packet.readBool();

            if( accepted )
            {
                _LOG( "Connection accepted!" );
                mStatusBits[ESB_CONNECTED] = true;
                mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE2] = false;
                mSelfInfo.statusBits[ECSB_WAIT_FOR_INITIAL_INFO_PACKET] = true;

                boost::asio::ip::udp::endpoint endpoint = packet.getEndpoint();
                mMessenger->setRemoteAddress( endpoint.address().to_string(),
                                              endpoint.port() );

                mSelfInfo.id = id;
                _LOG( "New ClientID", mSelfInfo.id );
            }
            else
            {
                _LOG( "Connection denied!" );
                mStatusBits[ESB_WAIT_FOR_CONNECTION_PHASE2] = false;
            }
        }
        break;
    }
    case EEAID_ALIVE_REQUEST:
    {
        if( mStatusBits[ESB_CONNECTED] )
            return serialize( EEAID_ALIVE_RESPOND );
        break;
    }
    case EEAID_ITEMINFO_REQUEST:
    {
        uint32_t uuid = packet.readUInt32();

        NetworkSyncable *syncable = NetworkSyncable::getObject( uuid );
        if( syncable )
            return syncable->serialize( EAID_CREATE );

        break;
    }
    case EEAID_CONNECTIONINFO_SEND:
    {
        if( mSelfInfo.statusBits[ECSB_WAIT_FOR_INITIAL_INFO_PACKET] )
        {
            mSelfInfo.initializationInfo.totalPlayers = packet.readUInt8();
            mSelfInfo.initializationInfo.totalItems = packet.readUInt32();
            mSelfInfo.initializationInfo.curPlayers = 0;
            mSelfInfo.initializationInfo.curItems = 0;
            mSelfInfo.statusBits[ECSB_WAIT_FOR_INITIAL_INFO_PACKET] = false;

            _LOG( "Info Packet received" );
            _LOG( "Num players", mSelfInfo.initializationInfo.totalPlayers );
            _LOG( "Num items", mSelfInfo.initializationInfo.totalItems );
        }
        break;
    }
    default:
    {
        break;
    }
    }
    return boost::none;
}

void ExploreServer::serializeInternalServer( NetworkSyncablePacket &packet,
                                             uint8_t actionID )
{
    switch( actionID )
    {
    case EEAID_SERVERINFO_RESPOND:
    {
        packet.writeString( mSelfInfo.host.hostName );
        packet.writeUInt8( mSelfInfo.host.serverMaxPlayers );
        packet.writeUInt8( mSelfInfo.host.serverConnectedPlayers );
        break;
    }
    case EEAID_CONNECTION_RESPOND:
    {
        packet.writeUInt32( nextClientID() - 1 );
        break;
    }
    default:
    {
        break;
    }
    }
}

void ExploreServer::serializeInternalClient( NetworkSyncablePacket &packet,
                                             uint8_t actionID )
{
    switch( actionID )
    {
    case EEAID_CONNECTION_REQUEST:
    {
        packet.writeString( mSelfInfo.host.hostName );
        packet.writeString( mSelfInfo.host.passwordHash );
        break;
    }
    case EEAID_CONNECTION_QUIT:
    {
        packet.writeUInt32( mSelfInfo.id );
        packet.writeUInt8( 0 ); //TODO: Add reason code
        break;
    }
    case EEAID_ALIVE_RESPOND:
    {
        packet.writeUInt32( mSelfInfo.id );
        break;
    }
    case EEAID_INITIALIZATION_FINISH:
    {
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

    std::map<uint32_t,ClientInfo>::iterator x = mClientIDMap.end();
    x--;
    return x->first + 1;
}

void ExploreServer::removeClient( const ClientInfo &client )
{
    //Check if client exists
    if( mClientIDMap.find( client.id ) == mClientIDMap.end() )
        return;

    mExplore->getExploreGame()->getWorldPlayer()->destroyChild(
                client.playerUID );

    mClientIDMap.erase( client.id );
}

void ExploreServer::handleInitPackets()
{
    if( mSelfInfo.statusBits[ECSB_WAIT_FOR_INITIAL_INFO_PACKET] )
        return;

    bool readyToReceive = mExplore->getGameState() == EGS_GAME
            || mStatusBits[ESB_CONNECTED];

    //Create new items and distribute them if this is a server
    if( mMessenger->hasPacketsInQueue() && readyToReceive )
    {
        NetworkSyncablePacket packet = mMessenger->nextPacket( false );

        //FIXME: This should not be needed; handling of oversupplied packets should
        //be done by NetworkMessenger
        if( NetworkSyncable::getObject( packet.getUID() ) )
        {
            _LOG( "INFO: Oversupplied packet with UID", packet.getUID() );
            mMessenger->popPacket();
            return;
        }

        if( !mSelfInfo.statusBits[ECSB_PLAYERS_CREATED]
                && packet.getTypeID() == ENTI_ITEM )
        {
            mMessenger->reQueuePacket();
        }
        else if( packet.getTypeID() == ENTI_ITEM )
        {
            mMessenger->popPacket();
            if( packet.getActionID() == EAID_CREATE )
                ItemFactory::create( mExplore, packet );
            else
            {
                NetworkSyncablePacket p = serialize( EEAID_ITEMINFO_REQUEST );
                p.writeUInt32( packet.getUID() );
                send( p );
            }

            mSelfInfo.initializationInfo.curItems++;

            if( mSelfInfo.initializationInfo.curItems >=
                    mSelfInfo.initializationInfo.totalItems )
            {
                mSelfInfo.statusBits[ECSB_ITEMS_CREATED] = true;
                mSelfInfo.statusBits[ECSB_INITIALIZED] = true;

                //Announce that we are ready
                send( serialize( EEAID_INITIALIZATION_FINISH, true ) );
            }
        }
        else if( packet.getTypeID() == ENTI_PLAYER )
        {
            WorldPlayerPtr world =
                    mExplore->getExploreGame()->getWorldPlayer();

            if( !world )
                mMessenger->reQueuePacket();
            else
            {
                mMessenger->popPacket();
                uint32_t clientID = packet.readUInt32();
                VisualPlayerPtr player;

                if( clientID == mSelfInfo.id )
                {
                    player.reset( new LocalPlayer( mExplore, world ),
                                  specialDeleters::NullDeleter() );
                    mSelfInfo.playerUID = packet.getUID();
                }
                else
                {
                    player.reset( new VisualPlayer( mExplore, world ),
                                  specialDeleters::NullDeleter() );
                }
                player->setClientID( clientID );
                player->deserialize( packet );

                //Set local player
                if( clientID == mSelfInfo.id )
                {
                    world->setLocalPlayer(
                                *( world->getChild( packet.getUID() ) ) );
                }

                mSelfInfo.initializationInfo.curPlayers++;

                if( mSelfInfo.initializationInfo.curPlayers >=
                        mSelfInfo.initializationInfo.totalPlayers )
                {
                    mSelfInfo.statusBits[ECSB_PLAYERS_CREATED] = true;
                }
            }
        }
        else if( packet.getTypeID() == ENTI_WORLD )
        {
            mMessenger->popPacket();
            WorldPlayerPtr world( new WorldPlayer( mExplore ) );
            mExplore->getExploreGame()->setWorldPlyer( world );
            mSelfInfo.initializationInfo.curPlayers++;

            if( mSelfInfo.initializationInfo.curPlayers >=
                    mSelfInfo.initializationInfo.totalPlayers )
            {
                mSelfInfo.statusBits[ECSB_PLAYERS_CREATED] = true;
            }
        }

        //If we are a server, distribute the new whatever to all clients
        if( mStatusBits[ESB_SERVER] )
        {
            send( packet );
        }
    }
}

ExploreServer::ClientInfo::ClientInfo()
    : id( 0 ),
      playerUID( 0 ),
      statusBits( ECSB_COUNT )
{
}
