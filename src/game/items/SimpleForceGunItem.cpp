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


#include <items/SimpleForceGunItem.h>
#include <players/LocalPlayer.h>
#include <ItemFactory.h>
#include <engine/EntityTools.h>
#include <engine/PathTools.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

int SimpleForceGunItem::sRegisterDummy( ItemFactory::registerItem<SimpleForceGunItem>( "SimpleForceGun" ) );

SimpleForceGunItem::SimpleForceGunItem( ExplorePtr explore, IPlayerPtr owner,
                                        PropTreePtr properties, const std::string &basePath )
    : Item( explore, owner, properties, basePath )
{
    mRayDistance = mProperties->get( "Item.RayDistance", 10.f );
    mForceMultiplicator = mProperties->get( "Item.ForceMultiplicator", 100.f );
    mDamage = mProperties->get( "Item.Damage", 1.f );
}

void SimpleForceGunItem::startActionInternal( uint8_t actionID )
{
    Item::startActionInternal( actionID );
    if( actionID == EIAID_FIRST_ACTION )
        shoot( true );
    else if( actionID == EIAID_SECOND_ACTION )
        shoot( false );
}

void SimpleForceGunItem::shoot( bool forward )
{
    //FIXME:search better way
    LocalPlayer* owner = static_cast<LocalPlayer*>( getOwner().get() );
    EntityTools::RayData rayData;

    rayData.ray.start = *( owner->getEntity()->getPosition() )
            + vector3df( 0.f, 1.f, 0.f );
    rayData.ray.end = rayData.ray.start + owner->rotateToDirection(
                vector3df( 0.f, 0.f, mRayDistance ) );

    boost::optional<Entity*> e =
            EntityTools::getFirstEntityInRay( mBulletWorld, rayData );

    if( e )
    {
        Entity *entity = *e;
        scene::IParticleSystemSceneNode *node = mDevice->getSceneManager()->addParticleSystemSceneNode(
                    false, 0, -1, rayData.outPoint );

        node->setEmitter( node->createPointEmitter( rayData.outNormal *= 0.01f, 10, 20,
                                                    video::SColor(), video::SColor(),
                                                    100, 500, 45 ) );
        node->addAffector( node->createGravityAffector() );
        node->addAffector( node->createFadeOutParticleAffector( video::SColor( 0, 0, 0, 0 ), 100 ) );
        node->setMaterialFlag( video::EMF_LIGHTING, false );
        node->addAnimator( mDevice->getSceneManager()->createDeleteAnimator( 500 ) );

        node->getEmitter()->setMinStartSize( dimension2df( 0.1f, 0.1f ) );
        node->getEmitter()->setMaxStartSize( dimension2df( 0.2f, 0.2f ) );
        node->getEmitter()->setMinStartColor( video::SColor( 255, 0, 0, 0 ) );
        node->getEmitter()->setMaxStartColor( video::SColor( 100, 100, 100, 100 ) );

        entity->getRigidBody()->activate();
        vector3df pos = *entity->getPosition();
        rayData.ray.start = ( rayData.ray.end - pos ) * mForceMultiplicator;

        if( !forward )
            rayData.ray.start *= -1.f;

        rayData.ray.end = rayData.outPoint - pos;

        entity->getRigidBody()->applyImpulse( VectorConverter::bt( rayData.ray.start ),
                                              VectorConverter::bt( rayData.ray.end ) );

        Item *hitItem = Item::getItemFromEntity( entity );

        if( hitItem )
            hitItem->modifyHitPoints( -mDamage );
    }
}
