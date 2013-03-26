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

#include <engine/PythonBinder.h>
#include <engine/NetworkMessenger.h>

PYTHONBINDER_REGISTER_MODULE( NetworkMessenger )
{
    using namespace boost::python;
    scope NM = class_<NetworkMessenger, NetworkMessengerPtr, boost::noncopyable>( "NetworkMessenger", no_init )
        .def( "send", &NetworkMessenger::send )
        .def( "checkedSend", &NetworkMessenger::checkedSend )
        .def( "sendTo",
              (void(NetworkMessenger::*)( const NetworkSyncablePacket&,
                                          const std::string&,
                                          const int&)) &NetworkMessenger::sendTo )
        .def( "checkedSendTo",
              (void(NetworkMessenger::*)( const NetworkSyncablePacket&,
                                          const uint8_t&)) &NetworkMessenger::checkedSendTo )
        .def( "hasPacketsInQueue", &NetworkMessenger::hasPacketsInQueue )
        .def( "nextPacket", &NetworkMessenger::nextPacket )
        .def( "bind", &NetworkMessenger::bind );

    class_<NetworkMessenger::Connection>( "Connection" );
}
