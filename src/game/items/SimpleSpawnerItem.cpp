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
#include "../IPlayer.h"
#include <engine/EntityTools.h>
#include <engine/IrrlichtTools.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

int SimpleSpawnerItem::sRegisterDummy =
        ItemFactory::registerItem<SimpleSpawnerItem>( "SimpleSpawner" );

SimpleSpawnerItem::SimpleSpawnerItem( ExplorePtr explore, IPlayerPtr owner,
                                      PropTreePtr properties,
                                      const std::string &basePath )
    : Item( explore, owner, properties, basePath ),
      mCurItem( 0 )
{
    loadSpawnableList();
    createPlacingMarkers();
}

void SimpleSpawnerItem::startActionInternal( uint8_t actionID )
{
    Item::startActionInternal( actionID );
    if( actionID == EIAID_FIRST_ACTION )
        spawn( false );
    else if( actionID == EIAID_SECOND_ACTION )
        spawn( true );
    else if( actionID == EAID_UPDATE )
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

    props->put( "Entity.Body.Position", mSpawnPoint );

    ItemPtr item( ItemFactory::create(
                      mExplore, getOwner(),
                      props, mBasePath ) );

    mSpawnedItems.push_back( item );
}

void SimpleSpawnerItem::update()
{
    if( !mActivated )
        return;

    IVideoDriverPtr driver = mDevice->getVideoDriver();

    vector3df hitPoint, normal, start, end;
    IPlayerPtr owner = getOwner();
    start = owner->getEntity()->getSceneNode()->getAbsolutePosition();
    end = owner->rotateToDirection( vector3df( 0.f, 0.f, 5.f ) ) + start;

    boost::optional<Entity*> e =
            EntityTools::getFirstEntityInRay( mExplore->getBulletWorld(),
                                              line3df( start, end ),
                                              hitPoint, normal );

    if( e )
    {
        IMeshSceneNode *node = static_cast<IMeshSceneNode*>(
                    mPlacingMarkers.at( 0 )->getSceneNode() );

        hitPoint += normal *
                ( node->getBoundingBox().getExtent() * node->getScale() ) / 2.f;

        matrix4 mat = IdentityMatrix;
        mat.setTranslation( hitPoint );
        mat.setScale( node->getScale() );

        driver->setTransform( ETS_WORLD, mat );
        driver->setMaterial( mMarkerMaterial );

        driver->drawMeshBuffer( node->getMesh()->getMeshBuffer( 0 ) );

        mSpawnPoint = hitPoint;

        mValidSpawnPoint = true;
    }
    else
        mValidSpawnPoint = false;


}

void SimpleSpawnerItem::loadSpawnableList()
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
                    _LOG( "Internal item added", name );
                }
            }
        }
    }
}

void SimpleSpawnerItem::createPlacingMarkers()
{
    for( std::vector<std::string>::iterator x = mSpawnableItems.begin();
         x != mSpawnableItems.end(); ++x )
    {
        PropTreePtr props = *ItemCache::instance()->getItemPropsCopy( *x );
        props->get_child( "Entity" ).erase( "Body" );
        props->put<bool>( "Entity.Node.Material.Visible", false );
        mPlacingMarkers.push_back( EntityPtr( new Entity(
                                                  mExplore->getIrrlichtDevice(),
                                                  mExplore->getBulletWorld(),
                                                  props, mBasePath ) ) );
    }

    SColor color = mProperties->get( "Item.PlacingMarkerColor", SColor( 255, 0, 255, 0 ) );
    bool zBuffer = mProperties->get( "Item.PlacingMarkerZBuffer", true );
    bool wire = mProperties->get( "Item.PlacingMarkerWireframe", true );
    bool useAlpha = mProperties->get( "Item.PlacingMarkerUseAlpha", false );

    if( useAlpha )
        mMarkerMaterial.MaterialType = EMT_TRANSPARENT_ALPHA_CHANNEL;
    else
        mMarkerMaterial.MaterialType = EMT_SOLID;

    mMarkerMaterial.DiffuseColor = color;
    mMarkerMaterial.SpecularColor = color;
    mMarkerMaterial.AmbientColor = color;
    mMarkerMaterial.EmissiveColor = color;
    mMarkerMaterial.ColorMask = ECP_ALL;
    mMarkerMaterial.ColorMaterial = ECM_NONE;
    mMarkerMaterial.ZBuffer = zBuffer;
    mMarkerMaterial.Wireframe = wire;
}
