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

#include "ItemFactory.h"
#include "ItemCache.h"
#include "IPlayer.h"
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

ItemCreatorMapPtr ItemFactory::sCreators( ItemFactory::sCreators );

class ItemFactoryBinder : public LuaBinder
{
public:
    void reg( LuaStatePtr state )
    {
        using namespace luabind;
        module( state.get() )
        [
                class_<ItemFactory>( "ItemFactory" )
                    .scope
                    [
                        def( "create", (Item*(*)(ExplorePtr, IPlayerPtr, std::string))&ItemFactory::create )
                    ]
        ];
    }
private:
    static int regDummy;
};
int ItemFactoryBinder::regDummy = LuaBinder::registerBinder( new ItemFactoryBinder );

Item *ItemFactory::create( ExplorePtr explore, IPlayerPtr owner, std::string fileName )
{
    boost::optional<PropTreePtr> cachedProps = ItemCache::instance()->getItemProps( fileName );

    fileName = PathTools::getAbsoluteFileNameFromFolder( fileName, "xml" );
    std::string basePath = PathTools::getBasePathFromFile( fileName );

    PropTreePtr properties;

    if( !cachedProps )
    {
        properties.reset( new boost::property_tree::ptree() );
        boost::property_tree::xml_parser::read_xml( fileName, *properties );
    }
    else
        properties.reset( new boost::property_tree::ptree( **cachedProps ) );

    std::string className = properties->get( "Item.Class", "Item" );

    ItemCreatorMap::iterator x = sCreators->find( className );

    if( x != sCreators->end() )
        return x->second->create( explore, owner, properties, basePath );
    else
    {
        _LOG( "Item class not found", className );
        return 0;
    }
}

Item *ItemFactory::create( ExplorePtr explore, IPlayerPtr owner, PropTreePtr props,
                           std::string basePath )
{
    std::string className = props->get( "Item.Class", "Item" );

    ItemCreatorMap::iterator x = sCreators->find( className );

    if( x != sCreators->end() )
        return x->second->create( explore, owner, props, basePath );
    else
    {
        _LOG( "Item class not found", className );
        return 0;
    }
}
