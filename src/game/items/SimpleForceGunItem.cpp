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


#include "SimpleForceGunItem.h"
#include "../IPlayer.h"
#include "../ItemFactory.h"
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

void SimpleForceGunItem::startAction( uint8_t actionID )
{
    Item::startAction( actionID );
    if( actionID == EIAID_FIRST_ACTION )
        shoot( true );
    else if( actionID == EIAID_SECOND_ACTION )
        shoot( false );
}

void SimpleForceGunItem::shoot( bool forward )
{
    line3df ray;
    IPlayerPtr owner = getOwner();
    ray.start = *( owner->getEntity()->getPosition() ) + vector3df( 0.f, 1.f, 0.f );
    ray.end = ray.start + owner->rotateToDirection( vector3df( 0.f, 0.f, mRayDistance ) );
    vector3df out, normal;
    boost::optional<Entity*> e =
            EntityTools::getFirstEntityInRay( mBulletWorld, ray, out, normal );

    if( e )
    {
        Entity *entity = *e;
        scene::IParticleSystemSceneNode *node = mDevice->getSceneManager()->addParticleSystemSceneNode(
                    false, 0, -1, out );

        node->setEmitter( node->createPointEmitter( normal *= 0.01f, 10, 20,
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
        ray.start = ( ray.end - pos ) * mForceMultiplicator;

        if( !forward )
            ray.start *= -1.f;

        ray.end = out - pos;

        entity->getRigidBody()->applyImpulse( VectorConverter::bt( ray.start ),
                                              VectorConverter::bt( ray.end ) );

        Item *hitItem = Item::getItemFromEntity( entity );

        if( hitItem )
            hitItem->modifyHitPoints( -mDamage );
    }
}
