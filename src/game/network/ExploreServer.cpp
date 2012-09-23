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

ExploreServer::ExploreServer()
    : mServerInfoAvailable( false )
{
}

void ExploreServer::requestServerInfo( NetworkMessenger *msg,
                                       const std::string &ip, const int &port )
{
    msg->sendTo( serialize( EAID_REQUEST_SERVERINFO ), ip, port );
}

bool ExploreServer::hasServerInfo() const
{
    return mServerInfoAvailable;
}

boost::optional<NetworkSyncablePacket> ExploreServer::deserializeInternal( NetworkSyncablePacket &packet )
{
    switch( packet.getActionID() )
    {
    case EAID_REQUEST_SERVERINFO:
        return serialize( EAID_SEND_SERVERINFO );
        break;
    case EAID_SEND_SERVERINFO:
        mServerInfoAvailable = true;
        break;
    default:
        break;
    }
    return boost::none;
}

void ExploreServer::serializeInternal( NetworkSyncablePacket &packet, uint8_t actionID )
{
}
