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

#include "NetworkMessenger.h"
#include "LoggerSingleton.h"
#include "NetworkSyncable.h"

using namespace boost::asio;

NetworkMessenger::NetworkMessenger( IOServicePtr ioService, PropTreePtr properties )
    : mIOService( ioService ),
      mProperties( properties )
{
    mServerIP = mProperties->get( "Server.IP", "127.0.0.1" );
    mPort = mProperties->get( "Server.Port", 6556 );
    mReceiveBuffer.resize( mProperties->get( "Server.BufferSize", 4096 ) );

    mSocket.reset( new ip::udp::socket( *mIOService.get() ) );

    bind( mPort );

    mRemoteEndpoint = ip::udp::endpoint(
                ip::address::from_string( mServerIP ), mPort );

    receive();
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

void NetworkMessenger::send( const NetworkSyncablePacket &packet )
{
    sendTo( packet, mRemoteEndpoint );
}

void NetworkMessenger::sendTo( const NetworkSyncablePacket &packet,
                               const ip::udp::endpoint &endpoint )
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
    packet.setEndpoint( mRemoteEndpoint );

    NetworkSyncable *syncable = NetworkSyncable::getObject( packet.getUID() );

    if( syncable )
    {
        boost::optional<NetworkSyncablePacket> replyPacket =
                syncable->deserialize( packet );

        if( replyPacket )
            send( *replyPacket );
    }
    else
        mPacketQueue.push( packet );

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
