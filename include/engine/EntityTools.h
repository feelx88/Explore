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


#ifndef ENTITYTOOLS_H
#define ENTITYTOOLS_H

#include <EngineTypedefs.h>
#include <Entity.h>

class APIEXPORT EntityTools
{
public:
    struct APIEXPORT RayData
    {
        RayData();
        irr::core::line3df ray;
        irr::core::vector3df outPoint, outNormal;
        unsigned short collisionFilterGroup, collisionFilterMask;
    };

    static boost::optional<Entity*> getFirstEntityInRay( IrrlichtDevicePtr device,
                                                         RayData &data );
    static boost::optional<Entity*> getFirstEntityInRay( BulletWorldPtr world,
                                                         RayData &data );
};

#endif // ENTITYTOOLS_H
