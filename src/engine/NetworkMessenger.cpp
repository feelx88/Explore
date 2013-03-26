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

#include <NetworkMessenger.h>
#include <LoggerSingleton.h>
#include <NetworkSyncable.h>

using namespace boost::asio;

NetworkMessenger::NetworkMessenger( IOServicePtr ioService, PropTreePtr properties )
    : mIOService( ioService ),
      mProperties( properties ),
      mCheckedSendTimer( *ioService.get() ),
      mCheckedSendTimerTimeout( 1000 )
{
    mServerIP = mProperties->get( "Server.IP", "127.0.0.1" );
    mPort = mProperties->get( "Server.Port", 6556 );
    mReceiveBuffer.resize( mProperties->get( "Server.BufferSize", 4096 ) );

    mSocket.reset( new ip::udp::socket( *mIOService.get() ) );

    bind( mPort );

    mRemoteEndpoint = ip::udp::endpoint(
                ip::address::from_string( mServerIP ), mPort );

    receive();
    checkedSendHandler();
}

NetworkMessenger::~NetworkMessenger()
{
    try
    {
        mSocket->shutdown( ip::udp::socket::shutdown_both );
    }
    catch( std::exception& )
    {
    }
    mSocket->close();
}

boost::optional<NetworkMessenger::ConnectionPtr> NetworkMessenger::connect(
        std::string host, int port )
{
    ip::udp::resolver res( *mIOService );
    ip::udp::resolver::iterator it =
            res.resolve( ip::udp::resolver::query( host, "" ) );

    if( it == ip::udp::resolver::iterator() )
    {
        return boost::none;
    }

    ConnectionPtr con( new Connection );
    UDPEndpoint endpoint = *it;
    con->endpoint = UDPEndpoint( endpoint.address(), port );
    con->id = getNextConnectionID();

    mConnections.insert( std::make_pair( con->id, con ) );
    mWaitingConnections.push_back( con->id );
    //checkedSendHandler();

    return con;
}

void NetworkMessenger::send( const NetworkSyncablePacket &packet )
{
    sendTo( packet, mRemoteEndpoint );
}

void NetworkMessenger::sendTo( const NetworkSyncablePacket &packet,
                               const NetworkMessenger::ConnectionPtr &connection )
{
    sendTo( packet, connection->id );
}

void NetworkMessenger::sendTo( const NetworkSyncablePacket &packet,
                               const uint8_t &connectionID )
{
    if( connectionID == 0 )
    {
        throw "Connection ID invalid!";
    }

    ConnectionPtr connection = mConnections[connectionID];

    sendTo( packet, connection->endpoint );
}

void NetworkMessenger::sendTo( const NetworkSyncablePacket &packet,
                               const UDPEndpoint &endpoint )
{
    mSocket->async_send_to( buffer( packet.serialize() ), endpoint, boost::bind(
                                &NetworkMessenger::sendHandler, this, _1, _2 ) );
}

void NetworkMessenger::sendTo( const NetworkSyncablePacket &packet,
                               const std::string &ip, const int &port )
{
    ip::udp::endpoint endpoint =
            ip::udp::endpoint( ip::address::from_string( ip ), port );

    sendTo( packet, endpoint );
}

void NetworkMessenger::checkedSend( const NetworkSyncablePacket &packet )
{
    //If there is at least one connection, send packet to the first available
    uint8_t id = packet.getConnectionID();
    if( id > 0 )
    {
        checkedSendTo( packet, mConnections[id] );
    }
}

void NetworkMessenger::checkedSendTo( const NetworkSyncablePacket &packet,
                                      const NetworkMessenger::ConnectionPtr &connection )
{
    checkedSendTo( packet, connection->id );
}

void NetworkMessenger::checkedSendTo( const NetworkSyncablePacket &packet,
                                      const uint8_t &connectionID )
{
    if( connectionID == 0 )
    {
        throw "Connection ID invalid!";
    }

    ConnectionPtr connection = mConnections[connectionID];

    if( connection )
    {
        NetworkSyncablePacket p( packet );

        if( p.getPacketType() == NetworkSyncablePacket::EPT_NORMAL )
            p.setPacketType( NetworkSyncablePacket::EPT_CHECKED );

        p.setConnectionID( connection->foreignID );
        p.setEndpoint( connection->endpoint );
        p.setSequenceCounter( connection->sequenceCounter );

        connection->checkedSendQueue.push( p );
        sendTo( p, connection->endpoint );
    }
}

boost::optional<NetworkMessenger::ConnectionPtr> NetworkMessenger::getConnection(
        uint8_t id )
{
    ConnectionMap::const_iterator it = mConnections.find( id );
    if( it != mConnections.end() )
    {
        return it->second;
    }
    return boost::none;
}

void NetworkMessenger::setCheckedSendTimeout(int timeoutMS)
{
    mCheckedSendTimerTimeout = timeoutMS;
}

int NetworkMessenger::checkedSendTimeout() const
{
    return mCheckedSendTimerTimeout;
}

bool NetworkMessenger::hasPacketsInQueue() const
{
    bool status = !mPacketQueue.empty();
    return status;
}

NetworkSyncablePacket NetworkMessenger::nextPacket( bool pop )
{
    NetworkSyncablePacket packet = mPacketQueue.front();
    if( pop )
        mPacketQueue.pop();
    return packet;
}

void NetworkMessenger::reQueuePacket()
{
    NetworkSyncablePacket &packet = mPacketQueue.front();
    mPacketQueue.pop();
    mPacketQueue.push( packet );
}

void NetworkMessenger::popPacket()
{
    mPacketQueue.pop();
}

void NetworkMessenger::setRemoteAddress( const std::string &address, const int &port )
{
    mServerIP = address;
    mPort = port;

    mRemoteEndpoint = ip::udp::endpoint(
                ip::address::from_string( mServerIP ), mPort );
}

void NetworkMessenger::bind( int port )
{
    _LOG( "Trying to bind socket to port", port );

    try
    {
        mSocket->close();
        mSocket->open( ip::udp::v4() );
    }
    catch( ... )
    {
        //Socket not opened
    }

    try
    {
        mSocket->bind( ip::udp::endpoint( ip::udp::v4(), port ) );
    }
    catch( ... )
    {
        _LOG( "Server could not bind to port", mPort );
        mSocket->bind( ip::udp::endpoint( ip::udp::v4(), 0 ) );
    }

    receive();
}

void NetworkMessenger::receive()
{
    mSocket->async_receive_from( buffer( mReceiveBuffer ), mRemoteEndpoint,
                                     boost::bind( &NetworkMessenger::receiveHandler,
                                                  this, _1, _2 ) );
}

void NetworkMessenger::receiveHandler( const boost::system::error_code &error,
                                       size_t /*transferred*/ )
{
    if( error )
    {
        _LOG( error.message() );
        receive();
        return;
    }

    NetworkSyncablePacket packet( std::string( mReceiveBuffer.begin(),
                                               mReceiveBuffer.end() ) );

    ConnectionPtr connection;
    if( packet.getConnectionID() > 0 )
    {
        connection = mConnections[packet.getConnectionID()];
    }

    if( packet.getPacketType() == NetworkSyncablePacket::EPT_INITIALIZATION )
    {
        uint8_t foreignID = packet.getConnectionID();
        std::map<uint8_t, uint8_t>::iterator it =
                mForeignConnectionIDs.find( foreignID );

        //Create new connection if none is found with this foreignID
        if( it == mForeignConnectionIDs.end() )
        {
            connection.reset( new Connection );
            connection->endpoint = mRemoteEndpoint;
            connection->id = getNextConnectionID();
            connection->foreignID = foreignID;
            connection->connected = true;

            mConnections.insert( std::make_pair( connection->id, connection ) );
            mForeignConnectionIDs.insert(
                        std::make_pair( connection->foreignID, connection->id ) );
        }
        else
        {
            connection = mConnections[it->second];
        }

        NetworkSyncablePacket response( packet );
        response.setPacketType( NetworkSyncablePacket::EPT_INITIALIZATION_RESPONSE );
        response.setConnectionID( connection->id );
        response.writeUInt8( foreignID );

        sendTo( response, connection->endpoint );
    }
    else if( packet.getPacketType() ==
             NetworkSyncablePacket::EPT_INITIALIZATION_RESPONSE )
    {
        uint8_t id = packet.readUInt8();
        connection = mConnections[id];

        if( !connection->connected )
        {
            connection->foreignID = packet.getConnectionID();
            connection->connected = true;

            for( unsigned int x = 0; x < mWaitingConnections.size(); ++x )
            {
                if( mWaitingConnections.at( x ) == id )
                    mWaitingConnections.erase( mWaitingConnections.begin() + x );
            }
        }
    }
    else if( packet.getPacketType() == NetworkSyncablePacket::EPT_PINGBACK
            && connection )
    {
        //Only pingback received, delete from queue and return
        NetworkSyncablePacket firstInQueue = connection->checkedSendQueue.front();
        if( packet.getSequenceCounter() == firstInQueue.getSequenceCounter() )
        {
            connection->checkedSendQueue.pop();
            connection->sequenceCounter++;
        }
    }
    else
    {
        if( packet.getPacketType() == NetworkSyncablePacket::EPT_CHECKED
                && packet.getSequenceCounter() >= connection->sequenceCounter )
        {
            //pingback requested, send it back without body
            NetworkSyncablePacket pingback( packet );

            pingback.setPacketType( NetworkSyncablePacket::EPT_PINGBACK );
            pingback.clearBody();

            sendTo( packet, mRemoteEndpoint );
        }

        packet.setEndpoint( mRemoteEndpoint );

        NetworkSyncable *syncable = NetworkSyncable::getObject( packet.getUID() );

        if( syncable )
        {
            boost::optional<NetworkSyncablePacket> replyPacket =
                    syncable->deserialize( packet );

            if( replyPacket )
            {
                if( replyPacket->getPacketType() ==
                        NetworkSyncablePacket::EPT_CHECKED )
                    checkedSend( *replyPacket );
                else
                    send( *replyPacket );
            }
        }
        else
            mPacketQueue.push( packet );
    }

    receive();
}

void NetworkMessenger::sendHandler( const boost::system::error_code &error, size_t )
{
    if( error )
    {
        _LOG( error.message() );
        return;
    }
}

void NetworkMessenger::checkedSendHandler()
{
    //Send the first (not ack'ed) entry in each queue if there is one
    foreach_( ConnectionMap::value_type& entry, mConnections )
    {
        ConnectionPtr &connection = entry.second;
        if( !connection->checkedSendQueue.empty() )
        {
            NetworkSyncablePacket& packet = connection->checkedSendQueue.front();
            sendTo( packet, connection->endpoint );
        }
    }

    foreach_( uint8_t &id, mWaitingConnections )
    {
        NetworkSyncablePacket packet( 0, 0, 0, "" );
        packet.setConnectionID( id );
        packet.setPacketType( NetworkSyncablePacket::EPT_INITIALIZATION );
        sendTo( packet, mConnections[id]->endpoint );
    }

    mCheckedSendTimer.expires_from_now(
                boost::posix_time::milliseconds( mCheckedSendTimerTimeout ) );
    mCheckedSendTimer.async_wait(
                boost::bind( &NetworkMessenger::checkedSendHandler, this ) );
}

uint8_t NetworkMessenger::getNextConnectionID() const
{
    if( mConnections.empty() )
        return 1;

    ConnectionMap::const_iterator it = mConnections.end();
    it--;

    return it->first + 1;
}


NetworkMessenger::Connection::Connection()
    : id( 0 ),
      foreignID( 0 ),
      sequenceCounter( 0 ),
      connected( false )
{
}
