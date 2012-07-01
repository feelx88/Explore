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

#include "SimpleSpawnerItem.h"
#include "../ItemFactory.h"
#include "../ItemCache.h"
#include "../Player.h"
#include <engine/EntityTools.h>
#include <engine/IrrlichtTools.h>

using namespace irr;
using namespace core;
using namespace video;

int SimpleSpawnerItem::sRegisterDummy =
        ItemFactory::registerItem<SimpleSpawnerItem>( "SimpleSpawner" );

SimpleSpawnerItem::SimpleSpawnerItem( ExplorePtr explore, PlayerPtr owner,
                                      PropTreePtr properties,
                                      const std::string &basePath )
    : Item( explore, owner, properties, basePath ),
      mCurItem( 0 )
{
    boost::property_tree::ptree spawnables = mProperties->get_child( "Item.CanSpawn" );

    for( boost::property_tree::ptree::iterator x = spawnables.begin();
         x != spawnables.end(); ++x )
    {
        if( x->first == "InternalItem" )
        {
            std::string name = x->second.data();

            for( boost::property_tree::ptree::iterator y = mProperties->begin();
                 y != mProperties->end(); ++y )
            {
                if( y->first == "InternalItem" && y->second.get( "<xmlattr>.Name", "" ) == name )
                {
                    mSpawnableItems.push_back( name );

                    PropTreePtr props( new boost::property_tree::ptree( y->second ) );

                    ItemCache::instance()->addItem( name, props );
                    mItemHalfExtents.push_back( calculateHalfExtentsFromItem( props ) );
                    _LOG( "Internal item added", name );
                }
            }
        }
    }


}

void SimpleSpawnerItem::startAction( int actionID )
{
    if( actionID == EIA_FIRST_ACTION )
        spawn( false );
    else if( actionID == EIA_SECOND_ACTION )
        spawn( true );
    else if( actionID == EIA_UPDATE_ACTION )
        update();

}

void SimpleSpawnerItem::spawn( bool zeroMass )
{
    if( !mValidSpawnPoint )
        return;

    PropTreePtr props = *ItemCache::instance()->getItemPropsCopy(
                mSpawnableItems.at( mCurItem ) );

    if( zeroMass )
        props->put( "Entity.Body.Mass", 0.f );

    Item *item( ItemFactory::create(
                mExplore, mOwner,
                props, "" ) );

    item->getEntities()->getEntity( 0 )->setPosition( mSpawnPoint );
    mSpawnedItems.push_back( item );
}

void SimpleSpawnerItem::update()
{
    if( !mActivated )
        return;

    IVideoDriverPtr driver = mDevice->getVideoDriver();

    vector3df hitPoint, normal, start, end;
    start = mOwner->getEntity()->getSceneNode()->getAbsolutePosition();
    end = mOwner->rotateToDirection( vector3df( 0.f, 0.f, 5.f ) ) + start;

    boost::optional<Entity*> e =
            EntityTools::getFirstEntityInRay( mExplore->getBulletWorld(),
                                              line3df( start, end ),
                                              hitPoint, normal );

    vector3df halfExtents = mItemHalfExtents.at( mCurItem );

    if( e )
    {
        normal = normal * halfExtents;
        aabbox3df box( hitPoint - halfExtents + normal,
                       hitPoint + halfExtents + normal );

        mSpawnPoint = box.getCenter();

        driver->setTransform( ETS_WORLD, IdentityMatrix );
        driver->setMaterial( driver->getMaterial2D() );
        driver->draw3DBox( box, SColor( 255, 0, 255, 0 ) );
        mValidSpawnPoint = true;
    }
    else
        mValidSpawnPoint = false;


}

vector3df SimpleSpawnerItem::calculateHalfExtentsFromItem(PropTreePtr props)
{
    //TODO: calculation
    return props->get( "Item.HalfExtents", vector3df() );
}
