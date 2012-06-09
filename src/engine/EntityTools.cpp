#include "EntityTools.h"
#include "VectorConverter.h"

boost::optional<Entity*> EntityTools::getFirstEntityInRay( IrrlichtDevicePtr device,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint )
{
    ISceneCollisionManagerPtr colMgr = device->getSceneManager()->getSceneCollisionManager();

    irr::core::triangle3df outTri;
    ISceneNodePtr node =
            colMgr->getSceneNodeAndCollisionPointFromRay( ray, outPoint, outTri );

    if( node )
        return Entity::getEntity( node );
    else
        return boost::none;
}

boost::optional<Entity*> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint )
{
    btVector3 from( VectorConverter::bt( ray.start ) ),
            to( VectorConverter::bt( ray.end ) );
    btCollisionWorld::ClosestRayResultCallback callback( from, to );
    world->rayTest( from, to, callback );

    outPoint = VectorConverter::irr( callback.m_hitPointWorld );

    btRigidBody *body = static_cast<btRigidBody*>( callback.m_collisionObject );

    if( body )
        return Entity::getEntity( body );
    else
        return boost::none;
    }
