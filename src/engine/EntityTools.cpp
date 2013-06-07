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


#include <EntityTools.h>
#include <VectorConverter.h>

using namespace irr;
using namespace core;

boost::optional<Entity*> EntityTools::getFirstEntityInRay( IrrlichtDevicePtr device,
                                                           RayData &data )
{
    ISceneCollisionManagerPtr colMgr = device->getSceneManager()->getSceneCollisionManager();

    irr::core::triangle3df outTri;
    ISceneNodePtr node =
            colMgr->getSceneNodeAndCollisionPointFromRay( data.ray, data.outPoint, outTri );

    data.outNormal = outTri.getNormal();

    if( node )
        return Entity::getEntity( node );
    else
        return boost::none;
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                           RayData &data )
{
    btVector3 from( VectorConverter::bt( data.ray.start ) ),
            to( VectorConverter::bt( data.ray.end ) );
    btCollisionWorld::AllHitsRayResultCallback callback( from, to );
    callback.m_collisionFilterGroup = data.collisionFilterGroup;
    callback.m_collisionFilterMask = data.collisionFilterMask;
    world->rayTest( from, to, callback );

    if( !callback.hasHit() )
    {
        return boost::none;
    }

    float nearestHit = std::numeric_limits<float>::max();
    boost::optional<Entity*> entity;
    vector3df outPoint, outNormal;

    for( int x = 0; x < callback.m_collisionObjects.size(); ++x )
    {
        //TODO: maybe use m_hitFraction instead of calculating distance?
        float hitDistanceSQ = ( VectorConverter::irr( callback.m_hitPointWorld[x] )
                              - data.ray.start ).getLengthSQ();

        const btRigidBody *body = static_cast<const btRigidBody*>(
                    callback.m_collisionObjects[x] );

        if( body )
        {
            boost::optional<Entity*> tmp = Entity::getEntity( body );
            if( tmp && hitDistanceSQ < nearestHit )
            {
                nearestHit = hitDistanceSQ;
                outPoint = VectorConverter::irr( callback.m_hitPointWorld[x] );
                outNormal = VectorConverter::irr( callback.m_hitNormalWorld[x] );
                entity = tmp;
            }
        }
    }

    if( entity )
    {
        data.outPoint = outPoint;
        data.outNormal = outNormal;
        return entity;
    }
    else
    {
        return boost::none;
    }
}


EntityTools::RayData::RayData()
    : collisionFilterGroup( std::numeric_limits<unsigned short>::max() ),
      collisionFilterMask( std::numeric_limits<unsigned short>::max() )
{
}
