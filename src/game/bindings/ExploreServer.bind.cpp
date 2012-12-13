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

#include <engine/LuaBinder.h>
#include "../network/ExploreServer.h"

//LUABINDER_REGISTER_MODULE_START( ExploreServerBinder )
BOOST_PYTHON_MODULE( ExploreServer )
{
    using namespace boost::python;
    class_<ExploreServer>( "ExploreServer" )
        .def( "setServerMode", &ExploreServer::setServerMode )
        .def( "requestServerInfo", &ExploreServer::requestServerInfo )
        .def( "hasServerInfo", &ExploreServer::hasServerInfo )
        .def( "nextServerInfo", &ExploreServer::nextServerInfo )
        .def( "requestConnection", &ExploreServer::requestConnection )
        .def( "getNetworkMessenger", &ExploreServer::getNetworkMessenger )
        .def( "setSelfInfo", &ExploreServer::setSelfInfo )
        .def( "serialize", &ExploreServer::serialize )
        .def( "send", &ExploreServer::send )
        .def( "setUpdateInterval", &ExploreServer::setUpdateInterval )
        .def( "updateInterval", &ExploreServer::updateInterval )
        .def( "setClientTimeout", &ExploreServer::setClientTimeout )
        .def( "clientTimeout", &ExploreServer::clientTimeout )
        .enum_( "E_ACTIONID" )
        [
            value( "ESAID_ACK", ExploreServer::ESAID_ACK ),
            value( "ESAID_NAK", ExploreServer::ESAID_NAK ),
            value( "ESAID_REQUEST_SERVERINFO",
                   ExploreServer::ESAID_REQUEST_SERVERINFO ),
            value( "ESAID_REQUEST_CONNECTION",
                   ExploreServer::ESAID_REQUEST_CONNECTION ),
            value( "ESAID_REQUEST_IS_STILL_ALIVE",
                   ExploreServer::ESAID_REQUEST_IS_STILL_ALIVE )
        ]
        .scope
        [
            class_<ExploreServer::HostInfo>( "HostInfo" )
                .def( constructor<>() )
                .def_readwrite( "hostName",
                                &ExploreServer::HostInfo::hostName )
        ];
}
//LUABINDER_REGISTER_MODULE_END( ExploreServerBinder )
