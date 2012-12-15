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

#include "../engine/PythonBinder.h"
#include "../engine/NetworkSyncablePacket.h"

PYTHONBINDER_REGISTER_MODULE( NetworkSyncablePacket )
{
    using namespace boost::python;
    class_<NetworkSyncablePacket>( "NetworkSyncablePacket", init<int, int, int, std::string>() )
        .def( init<std::string>() )
        .def( "serialize", &NetworkSyncablePacket::serialize )
        .def( "getUID", &NetworkSyncablePacket::getUID )
        .def( "getTypeID", &NetworkSyncablePacket::getTypeID )
        .def( "getActionID", &NetworkSyncablePacket::getActionID )
        .def( "readUInt8", &NetworkSyncablePacket::readUInt8 )
        .def( "writeUInt8", &NetworkSyncablePacket::writeUInt8 )
        .def( "readUInt16", &NetworkSyncablePacket::readUInt16 )
        .def( "writeUInt16", &NetworkSyncablePacket::writeUInt16 )
        .def( "readUInt32", &NetworkSyncablePacket::readUInt32 )
        .def( "writeUInt32", &NetworkSyncablePacket::writeUInt32 )
        .def( "readUInt64", &NetworkSyncablePacket::readUInt64 )
        .def( "writeUInt64", &NetworkSyncablePacket::writeUInt64 )
        .def( "readInt8", &NetworkSyncablePacket::readInt8 )
        .def( "writeInt8", &NetworkSyncablePacket::writeInt8 )
        .def( "readInt16", &NetworkSyncablePacket::readInt16 )
        .def( "writeInt16", &NetworkSyncablePacket::writeInt16 )
        .def( "readInt32", &NetworkSyncablePacket::readInt32 )
        .def( "writeInt32", &NetworkSyncablePacket::writeInt32 )
        .def( "readInt64", &NetworkSyncablePacket::readInt64 )
        .def( "writeInt64", &NetworkSyncablePacket::writeInt64 )
        .def( "readString", &NetworkSyncablePacket::readString )
        .def( "writeString", &NetworkSyncablePacket::writeString )
        .def( "readFloat", &NetworkSyncablePacket::readFloat )
        .def( "writeFloat", &NetworkSyncablePacket::writeFloat )
        .def( "readBool", &NetworkSyncablePacket::readBool )
        .def( "writeBool", &NetworkSyncablePacket::writeBool );
}
