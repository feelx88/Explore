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

#ifndef NETWORKSYNCABLEHEADER_H
#define NETWORKSYNCABLEHEADER_H

#include <string>
#include <stdint.h>

class NetworkSyncableHeader
{
public:
    NetworkSyncableHeader( const std::string &data );

    uint32_t getUID() const;
    uint8_t getTypeID() const;
    std::string getBody() const;

private:
    uint32_t mUID;
    uint8_t mTypeID;
    std::string mBody;
};

#endif // NETWORKSYNCABLEHEADER_H
