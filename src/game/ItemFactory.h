/*
    Copyright 2012 Felix Müller.

    This file is part of %ProjectName%.

    %ProjectName% is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    %ProjectName% is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with %ProjectName%.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef ITEMFACTORY_H
#define ITEMFACTORY_H

#include "Item.h"
#include <engine/LoggerSingleton.h>

struct ItemCreatorBase;

typedef boost::shared_ptr<ItemCreatorBase> ItemCreatorPtr;
typedef boost::unordered::unordered_map<std::string, ItemCreatorPtr> ItemCreatorMap;
typedef boost::shared_ptr<ItemCreatorMap> ItemCreatorMapPtr;

struct ItemCreatorBase
{
    virtual Item *create( ExplorePtr explore, PlayerPtr owner, PropTreePtr properties,
                          const std::string &basePath ) = 0;
};

template <typename T>
struct ItemCreator : public ItemCreatorBase
{
    Item *create( ExplorePtr explore, PlayerPtr owner, PropTreePtr properties,
                  const std::string &basePath )
    {
        return new T( explore, owner, properties, basePath );
    }
};

class ItemFactory
{
public:

    static Item *create( ExplorePtr explore, PlayerPtr owner, std::string fileName );

    template <typename T>
    static int registerItem( std::string name )
    {
        if( !sCreators )
        {
            _LOG( "Initializing static creator map." );
            sCreators.reset( new ItemCreatorMap() );
        }

        sCreators->insert( std::make_pair( name, new ItemCreator<T>() ) );
        _LOG( "Item class registered", name );

        return 0;
    }

private:
    static ItemCreatorMapPtr sCreators;
};

#endif // ITEMFACTORY_H
