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

#include "ItemCache.h"
#include <engine/LoggerSingleton.h>
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

ItemCachePtr ItemCache::sInstance;

ItemCachePtr ItemCache::instance()
{
    if( !sInstance )
        sInstance.reset( new ItemCache() );

    return sInstance;
}

void ItemCache::addItem( std::string name )
{
    _LOG( "Adding Item to cache", name );

    PropTreePtr props( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                PathTools::getAbsoluteFileNameFromFolder( name, "xml" ), *props );

    mItems.insert( std::make_pair( name, props ) );
}

boost::optional<PropTreePtr> ItemCache::getItemProps( std::string name ) const
{
    PropMap::const_iterator x = mItems.find( name );

    if( x == mItems.end() )
    {
        _LOG( "Item not registered", name );
        return boost::none;
    }
    else
        return x->second;
}

ItemCache::ItemCache()
{
}
