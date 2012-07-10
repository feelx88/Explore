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
#include "IPlayer.h"
#include <engine/PathTools.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

EntityItemMap Item::sEntityItemMap;
int Item::sRegisterDummy( ItemFactory::registerItem<Item>( "Item" ) );

Item::Item( ExplorePtr explore, IPlayer *owner, PropTreePtr properties,
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

    mOwner->addOwnedItem( this );

    mEntities.reset( new EntityContainer( mDevice, mBulletWorld, mProperties ) );

    registerScripts();

    if( mProperties->get( "Item.AutoAddEntities", false ) )
        registerEntities();

    if( mProperties->get( "Item.AutoActivation", false ) )
        setActivationState( true );

    mHitPoints = mProperties->get( "Item.HitPoints", 1.f );
}

Item::~Item()
{
    if( mEntities )
    {
        for( EntityMap::const_iterator x = mEntities->getEntities().begin();
             x != mEntities->getEntities().end(); ++x )
        {
            sEntityItemMap.erase( x->second.get() );
        }
    }

    if( mGUI )
        mGUI->remove();
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

void Item::startAction( E_ITEM_ACTION actionID )
{
    ScriptMap::iterator x = mScripts.find( actionID );
    if( x == mScripts.end() )
        return;
    else
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

float Item::getHitPoints() const
{
    return mHitPoints;
}

void Item::setHitPoints( float hitPoints )
{
    mHitPoints = hitPoints;
}

void Item::modifyHitPoints( float difference )
{
    mHitPoints += difference;
}

void Item::setActivationState( bool state )
{
    mActivated = state;
}

bool Item::getActivationState() const
{
    return mActivated;
}

Item *Item::getItemFromEntity( Entity *entity )
{
    EntityItemMap::iterator x = sEntityItemMap.find( entity );

    if( x != sEntityItemMap.end() )
        return x->second;
    else
        return 0;
}

void Item::registerScripts()
{
    boost::optional<boost::property_tree::ptree&> itemTree =
        mProperties->get_child_optional( "Item" );

    if( !itemTree )
        return;

    for( boost::property_tree::ptree::iterator x = ( *itemTree ).begin();
         x != ( *itemTree ).end(); ++x )
    {
        if( x->first == "Script" )
        {
            E_ITEM_ACTION actionID =
                    x->second.get( "<xmlattr>.Action", EIA_FIRST_ACTION );
            LuaScriptPtr script( new LuaScript(
                                     mLua, x->second.data(),
                                     x->second.get( "<xmlattr>.File", false ) ) );
            mScripts.insert( std::make_pair( actionID, script ) );
        }
    }
}
