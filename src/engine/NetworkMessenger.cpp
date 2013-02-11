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

void NetworkMessenger::send( const NetworkSyncablePacket &packet )
{
    sendTo( packet, mRemoteEndpoint );
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

void NetworkMessenger::checkedSend( NetworkSyncablePacket &packet )
{
    checkedSendTo( packet, mRemoteEndpoint );
}

void NetworkMessenger::checkedSendTo( NetworkSyncablePacket &packet,
                                      const UDPEndpoint &endpoint )
{
    if( packet.getPingbackMode() == NetworkSyncablePacket::ENSPPM_NONE )
        packet.setPingbackMode( NetworkSyncablePacket::ENSPPM_REQUEST_PINGBACK );
    mCheckedSendPackets.insert( std::make_pair( packet.getUID(), std::make_pair( packet, endpoint ) ) );
    sendTo( packet, endpoint );
}

void NetworkMessenger::checkedSendTo( NetworkSyncablePacket &packet,
                                      const std::string &ip, const int &port )
{
    ip::udp::endpoint endpoint =
            ip::udp::endpoint( ip::address::from_string( ip ), port );
    checkedSendTo( packet, endpoint );
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

    if( packet.getPingbackMode() ==
             NetworkSyncablePacket::ENSPPM_PINGBACK )
    {
        //Only pingback received, delete from map and return
        typedef boost::unordered::unordered_multimap<uint32_t,
                std::pair<NetworkSyncablePacket, UDPEndpoint> > map_t;
        std::pair<map_t::iterator, map_t::iterator> found =
                mCheckedSendPackets.equal_range( packet.getUID() );

        for( map_t::iterator x = found.first; x != found.second; ++x )
        {
            NetworkSyncablePacket &p = x->second.first;
            if( p.getActionID() == packet.getActionID()
                    && p.getTypeID() == packet.getTypeID() )
                mCheckedSendPackets.erase( x );
        }
    }
    else
    {
        if( packet.getPingbackMode() ==
                NetworkSyncablePacket::ENSPPM_REQUEST_PINGBACK )
        {
            //pingback requested, send it back
            packet.setPingbackMode( NetworkSyncablePacket::ENSPPM_PINGBACK );
            checkedSendTo( packet, mRemoteEndpoint );
        }

        packet.setEndpoint( mRemoteEndpoint );

        NetworkSyncable *syncable = NetworkSyncable::getObject( packet.getUID() );

        if( syncable )
        {
            boost::optional<NetworkSyncablePacket> replyPacket =
                    syncable->deserialize( packet );

            if( replyPacket )
            {
                if( replyPacket->getPingbackMode() ==
                        NetworkSyncablePacket::ENSPPM_REQUEST_PINGBACK )
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
    typedef boost::unordered::unordered_multimap<uint32_t,
            std::pair<NetworkSyncablePacket, UDPEndpoint> > map_t;
    foreach_( map_t::value_type &pair, mCheckedSendPackets )
            sendTo( pair.second.first, pair.second.second );

    mCheckedSendTimer.expires_from_now(
                boost::posix_time::milliseconds( mCheckedSendTimerTimeout ) );
    mCheckedSendTimer.async_wait(
                boost::bind( &NetworkMessenger::checkedSendHandler, this ) );
}
