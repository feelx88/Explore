/*
    Copyright 2013 Felix Müller.

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

#include "NetworkTools.h"

StringVector NetworkTools::resolveName( const std::string &name, IOServicePtr io )
{
    //Allow calling method without io service object
    if( !io )
    {
        io.reset( new boost::asio::io_service() );
    }

    boost::asio::ip::udp::resolver res( *io.get() );
    boost::asio::ip::udp::resolver::iterator it =
            res.resolve( boost::asio::ip::udp::resolver::query( name, "" ) );

    StringVector ips;

    for( ; it != boost::asio::ip::udp::resolver::iterator(); ++it )
    {
        ips.push_back( static_cast<boost::asio::ip::udp::endpoint>( *it )
                       .address().to_string() );
    }

    return ips;
}

#include "NetworkTools.h"
