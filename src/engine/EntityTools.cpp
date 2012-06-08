#include "EntityTools.h"

boost::optional<EntityPtr> EntityTools::getFirstEntityInRay( IrrlichtDevicePtr device,
                                                             const irr::core::line3df &ray,
                                                             irr::core::vector3df &outPoint )
{
    ISceneCollisionManagerPtr colMgr = device->getSceneManager()->getSceneCollisionManager();

    irr::core::triangle3df outTri;
    ISceneNodePtr node =
            colMgr->getSceneNodeAndCollisionPointFromRay( ray, outPoint, outTri );

    return Entity::getEntity( node );
}

boost::optional<EntityPtr> EntityTools::getFirstEntityInRay( BulletWorldPtr world,
                                                             const irr::core::line3df &ray,
                                                             irr::core::vector3df &outPoint )
{
    //TODO:
}
