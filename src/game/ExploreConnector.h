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

#ifndef EXPLORECONNECTOR_H
#define EXPLORECONNECTOR_H

#include <engine/EngineTypedefs.h>
#include <boost/asio.hpp>

typedef boost::shared_ptr<boost::asio::io_service> IOServicePtr;
typedef boost::shared_ptr<boost::asio::ip::udp::socket> UDPSocketPtr;

class ExploreConnector
{
public:
    ExploreConnector( IOServicePtr ioService, PropTreePtr properties );

private:

    void send();
    void receive();

    void receiveHandler( const boost::system::error_code &error, size_t transferred );
    void sendHandler( const boost::system::error_code &error, size_t transferred );

    IOServicePtr mIOService;
    PropTreePtr mProperties;

    UDPSocketPtr mSocket;

    bool mIsServer;
    std::string mServerIP;
    int mPort;

    std::string mSendBuffer;
    std::vector<char> mReceiveBuffer;
    boost::asio::ip::udp::endpoint mRemoteEndpoint;
};

#endif // EXPLORECONNECTOR_H
