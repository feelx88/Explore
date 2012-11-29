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
    _LOG( "Loading Items from file", name );

    PropTreePtr props( new boost::property_tree::ptree() );

    boost::filesystem::path p( name );
    p /= "Manifest.xml";

    boost::property_tree::xml_parser::read_xml( p.string(), *props );
    props->put( "Item.BasePath", name );

    addItem( name, props.get() );

    for( boost::property_tree::ptree::iterator x = props->begin();
         x != props->end(); ++x )
    {
        if( x->first == "InternalItem" )
        {
            x->second.put( "Item.BasePath", name );
            addItem( name + ".Internal", &x->second );
        }
    }
}

void ItemCache::addItem( const std::string &name,
                         boost::property_tree::ptree *properties )
{
    PropTreePtr props( new boost::property_tree::ptree( *properties ) );

    std::string cacheID;
    try
    {
        cacheID = props->get<std::string>( "Item.CacheID" );
        _LOG( "Adding Item to cache", cacheID );
    }
    catch( boost::property_tree::ptree_error &error )
    {
        _LOG( "Error: Item.CacheID not found", name );
        throw error;
    }

    mItems.insert( std::make_pair( cacheID, props ) );
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

boost::optional<PropTreePtr> ItemCache::getItemPropsCopy( const std::string &name )
{
    boost::optional<PropTreePtr> props = getItemProps( name );

    if( props )
        return PropTreePtr( new boost::property_tree::ptree( **props ) );
    else
        return boost::none;
}

const PropMap &ItemCache::getAllItemProps() const
{
    return mItems;
}

ItemCache::ItemCache()
{
}
