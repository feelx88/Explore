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


#include <BulletTools.h>

RigidBodyPtr BulletTools::createRigidBodyPtr( BulletWorldPtr world, btRigidBody *body )
{
    return RigidBodyPtr( body, specialDeleters::RigidBodyDeleter( world ) );
}

CollisionObjectPtr BulletTools::createCollisionObjectPtr(BulletWorldPtr world, btCollisionObject *object)
{
    return CollisionObjectPtr( object, specialDeleters::CollisionObjectDeleter( world ) );
}
