#ifndef BULLETTOOLS_H
#define BULLETTOOLS_H

#include "EngineTypedefs.h"

class BulletTools
{
public:
    static RigidBodyPtr createRigidBodyptr( BulletWorldPtr world, btRigidBody *body );
};

#endif // BULLETTOOLS_H
