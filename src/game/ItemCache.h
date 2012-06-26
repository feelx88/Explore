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

#ifndef ITEMCACHE_H
#define ITEMCACHE_H

#include <engine/EngineTypedefs.h>
#include <boost/unordered_map.hpp>

class ItemCache;
typedef boost::shared_ptr<ItemCache> ItemCachePtr;
typedef boost::unordered::unordered_map<std::string, PropTreePtr> PropMap;

class ItemCache
{
public:
    static ItemCachePtr instance();

    void addItem( const std::string &name );
    void addItem( const std::string &name, PropTreePtr properties );
    boost::optional<PropTreePtr> getItemProps( const std::string &name ) const;
    boost::optional<PropTreePtr> getItemPropsCopy( const std::string &name );

    const PropMap &getAllItemProps() const;

private:
    ItemCache();

    PropMap mItems;

    static ItemCachePtr sInstance;
};

#endif // ITEMCACHE_H
