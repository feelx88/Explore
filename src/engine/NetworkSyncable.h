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

#ifndef NETWORKSYNCABLE_H
#define NETWORKSYNCABLE_H

#include "NetworkSyncablePacket.h"

#include <boost/optional.hpp>
#include <string>
#include <map>
#include <stdint.h>

class NetworkSyncable
{
public:
    struct UIDAlreadyUsedException
    {
        UIDAlreadyUsedException( uint32_t uid );
    };

    NetworkSyncable();
    NetworkSyncable( uint32_t uid, uint8_t typeID );
    virtual ~NetworkSyncable();

    uint32_t getUID() const;
    uint8_t getTypeID() const;

    boost::optional<NetworkSyncablePacket> deserialize(
            NetworkSyncablePacket &packet );
    NetworkSyncablePacket serialize( uint8_t actionID );

    static NetworkSyncable *getObject( uint32_t uid );

protected:
    virtual boost::optional<NetworkSyncablePacket> deserializeInternal(
            NetworkSyncablePacket &packet ) = 0;
    virtual void serializeInternal( NetworkSyncablePacket &packet,
                                    uint8_t actionID ) = 0;

    void setUID( uint32_t uid );
    void setTypeID( uint8_t typeID );

    uint32_t mUID;
    uint8_t mTypeID;

private:
    static uint32_t nextUID();
    static std::map<uint32_t, NetworkSyncable*> sUIDMap;
};

#endif // NETWORKSYNCABLE_H
