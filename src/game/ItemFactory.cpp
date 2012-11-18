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
#include "ExploreGame.h"
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>
#include <luabind/adopt_policy.hpp>

ItemCreatorMapPtr ItemFactory::sCreators( ItemFactory::sCreators );

LUABINDER_REGISTER_MODULE_START( ItemFactoryBinder )
    class_<ItemFactory>( "ItemFactory" )
        .scope
        [
            def( "create",
                 (ItemPtr(*)(ExplorePtr, IPlayerPtr, std::string))&ItemFactory::create )
        ]
LUABINDER_REGISTER_MODULE_END( ItemFactoryBinder )

ItemPtr ItemFactory::create( ExplorePtr explore, IPlayerPtr owner,
                             std::string fileName )
{
    boost::optional<PropTreePtr> cachedProps =
            ItemCache::instance()->getItemProps( fileName );

    std::string newFileName = PathTools::getAbsoluteFileNameFromFolder( fileName, "xml" );
    std::string basePath = PathTools::getBasePathFromFile( newFileName );

    PropTreePtr properties;

    if( !cachedProps )
    {
        properties.reset( new boost::property_tree::ptree() );
        boost::property_tree::xml_parser::read_xml( newFileName, *properties );
    }
    else
        properties.reset( new boost::property_tree::ptree( **cachedProps ) );

    std::string className = properties->get( "Item.Class", "Item" );

    ItemCreatorMap::iterator x = sCreators->find( className );

    if( x != sCreators->end() )
    {
        ItemPtr item( x->second->create( explore, owner, properties, basePath ) );
        owner->addOwnedItem( item );
        item->mFileName = fileName;
        return item;
    }
    else
    {
        _LOG( "Item class not found", className );
        return ItemPtr();
    }
}

ItemPtr ItemFactory::create( ExplorePtr explore, IPlayerPtr owner,
                             PropTreePtr props, std::string basePath )
{
    std::string className = props->get( "Item.Class", "Item" );

    ItemCreatorMap::iterator x = sCreators->find( className );

    if( x != sCreators->end() )
    {
        ItemPtr item( x->second->create( explore, owner, props, basePath ) );
        owner->addOwnedItem( item );
        item->mFileName = props->get( "Item.Name", "INTERNAL" );
        return item;
    }
    else
    {
        _LOG( "Item class not found", className );
        return ItemPtr();
    }
}

ItemPtr ItemFactory::create( ExplorePtr explore, NetworkSyncablePacket &packet )
{
    if( NetworkSyncable::getObject( packet.getUID() ) )
    {
        _LOG( "UID exists, maybe double transferred", packet.getUID() );
        return ItemPtr();
    }

    std::string fileName = packet.readString();
    /*uint32_t ownerID =*/ packet.readUInt32();
    IPlayerPtr owner = explore->getExploreGame()->getWorldPlayer(); //TODO: attach right IPlayer

    ItemPtr item = create( explore, owner, fileName );
    item->setUID( packet.getUID() );
    item->deserialize( packet );

    return item;
}
