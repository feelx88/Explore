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


#include "Item.h"
#include "ItemFactory.h"
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

EntityItemMap Item::sEntityItemMap;
int Item::sRegisterDummy( ItemFactory::registerItem<Item>( "Item" ) );

Item::Item( ExplorePtr explore, Player *owner, PropTreePtr properties,
            const std::string &basePath )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mBulletWorld( explore->getBulletWorld() ),
      mLua( explore->getLuaVM() ),
      mProperties( PropTreePtr( new boost::property_tree::ptree( *properties ) ) ),
      mBasePath( basePath ),
      mOwner( owner ),
      mGUI( 0 )
{
    loadIcon();

    mEntities.reset( new EntityContainer( mDevice, mBulletWorld, mProperties ) );

    if( mProperties->get( "Item.AutoAddEntities", false ) )
        create();
}

Item::~Item()
{
    if( mEntities )
    {
        for( EntityMap::const_iterator x = mEntities->getEntities().begin();
             x != mEntities->getEntities().end(); ++x )
        {
            sEntityItemMap.insert( std::make_pair( x->second.get(), this ) );
        }
    }
}

PropTreePtr Item::getProperties() const
{
    return mProperties;
}

EntityContainerPtr Item::getEntities() const
{
    return mEntities;
}

PlayerPtr Item::getOwner() const
{
    return mOwner;
}

void Item::create()
{
    for( boost::property_tree::ptree::iterator x = mProperties->begin();
         x != mProperties->end(); ++x )
    {
        if( x->first == "Script" )
        {
            LuaScriptPtr script( new LuaScript(
                                     mLua, x->second.data(),
                                     x->second.get( "<xmlattr>.File", false ) ) );
            mScripts.push_back( script );
        }
    }

    registerEntities();
}

void Item::registerEntities()
{
    for( EntityMap::const_iterator x = mEntities->getEntities().begin();
         x != mEntities->getEntities().end(); ++x )
    {
        sEntityItemMap.insert( std::make_pair( x->second.get(), this ) );
    }
}

void Item::loadIcon()
{
    std::string iconFileName = PathTools::getAbsolutePath( "defaultIcon.png" );
    iconFileName = mProperties->get( "Item.Icon", iconFileName );
    iconFileName = PathTools::getAbsolutePath( iconFileName, mBasePath );

    mIcon = mDevice->getVideoDriver()->getTexture( iconFileName.c_str() );
}


void Item::startAction( int actionID )
{
    mScripts.at( actionID )->exec();
}

void Item::setGUIVisible( bool visible )
{
    if( !mGUI )
        return;

    mGUI->setVisible( visible );
}

IGUIElementPtr Item::getGUIElement() const
{
    return mGUI;
}

ITexturePtr Item::getIcon() const
{
    return mIcon;
}
