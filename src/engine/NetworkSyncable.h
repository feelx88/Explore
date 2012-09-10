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

#include <string>
#include <map>
#include <stdint.h>
#include "NetworkSyncableHeader.h"

class NetworkSyncable
{
public:
    struct UIDAlreadyUsedException : public std::exception
    {
        UIDAlreadyUsedException( uint32_t uid );
        const char *what();
        uint32_t mUID;
    };

    NetworkSyncable();
    NetworkSyncable( uint32_t uid );
    virtual ~NetworkSyncable();

    virtual void deserialize( const std::string &data ) = 0;
    virtual std::string serialize() = 0;

protected:
    void setUID( uint32_t uid );

    uint32_t mUID;

private:
    static uint32_t nextUID();
    static std::map<uint32_t, NetworkSyncable*> sUIDMap;
};

#endif // NETWORKSYNCABLE_H
