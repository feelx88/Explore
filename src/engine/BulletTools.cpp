#include "BulletTools.h"

RigidBodyPtr BulletTools::createRigidBodyptr( BulletWorldPtr world, btRigidBody *body )
{
    return RigidBodyPtr( body, specialDeleters::RigidBodyDeleter( world ) );
}
