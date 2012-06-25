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

void ItemCache::addItem( const std::string &name )
{
    _LOG( "Adding Item to cache", name );

    PropTreePtr props( new boost::property_tree::ptree() );
    boost::property_tree::xml_parser::read_xml(
                PathTools::getAbsoluteFileNameFromFolder( name, "xml" ), *props );

    mItems.insert( std::make_pair( name, props ) );
}

void ItemCache::addItem( const std::string &name , PropTreePtr properties )
{
    _LOG( "Adding Item to cache", name );
    mItems.insert( std::make_pair( name, properties ) );
}

boost::optional<PropTreePtr> ItemCache::getItemProps( const std::string &name ) const
{
    PropMap::const_iterator x = mItems.find( name );

    if( x == mItems.end() )
    {
        _LOG( "Item not registered", name );
        return boost::none;
    }
    else
    {
        _LOG( "Loading Item from cache", name );
        return x->second;
    }
}

const PropMap &ItemCache::getAllItemProps() const
{
    return mItems;
}

ItemCache::ItemCache()
{
}
