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
                                                  const irr::core::line3df &ray )
{
    vector3df tmpPoint, tmpNormal;
    return getFirstEntityInRay( device, ray, tmpPoint, tmpNormal );
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                  const irr::core::line3df &ray )
{
    vector3df tmpPoint, tmpNormal;
    return getFirstEntityInRay( world, ray, tmpPoint, tmpNormal );
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( IrrlichtDevicePtr device,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint )
{
    vector3df tmpNormal;
    return getFirstEntityInRay( device, ray, outPoint, tmpNormal );
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint )
{
    vector3df tmpNormal;
    return getFirstEntityInRay( world, ray, outPoint, tmpNormal );
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( IrrlichtDevicePtr device,
                                                  const irr::core::line3df &ray,
                                                  irr::core::vector3df &outPoint,
                                                  irr::core::vector3df &outNormal )
{
    ISceneCollisionManagerPtr colMgr = device->getSceneManager()->getSceneCollisionManager();

    irr::core::triangle3df outTri;
    ISceneNodePtr node =
            colMgr->getSceneNodeAndCollisionPointFromRay( ray, outPoint, outTri );

    outNormal = outTri.getNormal();

    if( node )
        return Entity::getEntity( node );
    else
        return boost::none;
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                  const irr::core::line3df &ray,
                                                  irr::core::vector3df &outPoint,
                                                  irr::core::vector3df &outNormal )
{
    btVector3 from( VectorConverter::bt( ray.start ) ),
            to( VectorConverter::bt( ray.end ) );
    btCollisionWorld::ClosestRayResultCallback callback( from, to );
    world->rayTest( from, to, callback );

    outPoint = VectorConverter::irr( callback.m_hitPointWorld );
    outNormal = VectorConverter::irr( callback.m_hitNormalWorld );

    //FIXME: do not cast const away
    btRigidBody *body = static_cast<btRigidBody*>(
                const_cast<btCollisionObject*>( callback.m_collisionObject ) );

    if( body )
        return Entity::getEntity( body );
    else
        return boost::none;
}
