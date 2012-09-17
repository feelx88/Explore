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

#include "ExploreConnector.h"
#include <engine/LoggerSingleton.h>

using namespace boost::asio;

ExploreConnector::ExploreConnector( IOServicePtr ioService, PropTreePtr properties )
    : mIOService( ioService ),
      mProperties( properties )
{
    mIsServer = mProperties->get( "Server.Activated", false );
    mServerIP = mProperties->get( "Server.IP", "127.0.0.1" );
    mPort = mProperties->get( "Server.Port", 6556 );
    mReceiveBuffer.resize( mProperties->get( "Server.BufferSize", 512 ) );

    mSocket.reset( new ip::udp::socket( *mIOService.get() ) );
    mSocket->open( ip::udp::v4() );

    if( mIsServer )
        mSocket->bind( ip::udp::endpoint( ip::udp::v4(), mPort ) );
    else
        mRemoteEndpoint = ip::udp::endpoint(
                    ip::address::from_string( mServerIP ), mPort );

    receive();
}

void ExploreConnector::send( const NetworkSyncablePacket &packet )
{
    mSocket->async_send_to( buffer( packet.serialize() ), mRemoteEndpoint, boost::bind(
                                &ExploreConnector::sendHandler, this, _1, _2 ) );
}

bool ExploreConnector::hasPacketsInQueue() const
{
    return !mPacketQueue.empty();
}

NetworkSyncablePacket ExploreConnector::nextPacket()
{
    NetworkSyncablePacket packet = mPacketQueue.front();
    mPacketQueue.pop();
    return packet;
}

void ExploreConnector::receive()
{
    mSocket->async_receive_from( buffer( mReceiveBuffer ), mRemoteEndpoint,
                                     boost::bind( &ExploreConnector::receiveHandler,
                                                  this, _1, _2 ) );
}

void ExploreConnector::receiveHandler( const boost::system::error_code &error,
                                       size_t )
{
    if( error )
        return;

    NetworkSyncablePacket packet( std::string( mReceiveBuffer.begin(),
                                               mReceiveBuffer.end() ) );
    mPacketQueue.push( packet );

    receive();
}

void ExploreConnector::sendHandler(const boost::system::error_code &error, size_t )
{
    if( error )
        return;

    //TODO:Do somethin
}
