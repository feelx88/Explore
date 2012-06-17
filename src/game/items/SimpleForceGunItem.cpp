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
#include "../Player.h"
#include "../ItemFactory.h"
#include <engine/EntityTools.h>
#include <engine/PathTools.h>
#include <engine/VectorConverter.h>
#include <boost/property_tree/xml_parser.hpp>

using namespace irr;
using namespace core;

int SimpleForceGunItem::sRegisterDummy( ItemFactory::registerItem<SimpleForceGunItem>( "SimpleForceGun" ) );

SimpleForceGunItem::SimpleForceGunItem( ExplorePtr explore, PlayerPtr owner,
                                        PropTreePtr properties, const std::string &basePath )
    : Item( explore, owner, properties, basePath )
{
    mRayDistance = mProperties->get( "Item.RayDistance", 10.f );
    mForceMultiplicator = mProperties->get( "Item.ForceMultiplicator", 100.f );

    loadIcon();
}

void SimpleForceGunItem::startAction( int actionID )
{
    if( actionID == EIA_FIRST_ACTION )
        shoot( true );
    else if( actionID == EIA_SECOND_ACTION )
        shoot( false );
}

void SimpleForceGunItem::shoot( bool forward )
{
    line3df ray;
    ray.start = *( mOwner->getEntity()->getPosition() ) + vector3df( 0.f, 1.f, 0.f );
    ray.end = ray.start + mOwner->rotateToDirection( vector3df( 0.f, 0.f, mRayDistance ) );
    vector3df out;
    boost::optional<Entity*> e =
            EntityTools::getFirstEntityInRay( mBulletWorld, ray, out );

    if( e )
    {
        ( *e )->getRigidBody()->activate();
        vector3df pos = *( *e )->getPosition();
        ray.start = ( ray.end - pos ) * mForceMultiplicator;

        if( !forward )
            ray.start *= -1.f;

        ray.end = out - pos;

        ( *e )->getRigidBody()->applyImpulse( VectorConverter::bt( ray.start ),
                                              VectorConverter::bt( ray.end ) );
    }
}
