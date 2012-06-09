#ifndef ENTITYTOOLS_H
#define ENTITYTOOLS_H

#include "EngineTypedefs.h"
#include "Entity.h"

class EntityTools
{
public:
    static boost::optional<Entity*> getFirstEntityInRay( IrrlichtDevicePtr device,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint );
    static boost::optional<Entity*> getFirstEntityInRay( BulletWorldPtr world,
                                                           const irr::core::line3df &ray,
                                                           irr::core::vector3df &outPoint );
};

#endif // ENTITYTOOLS_H
