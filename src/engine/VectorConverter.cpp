/*
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


#include "VectorConverter.h"

irr::core::vector3df VectorConverter::irr( const btVector3 &btVec )
{
    return irr::core::vector3df( btVec.getX(), btVec.getY(), btVec.getZ() );
}


btVector3 VectorConverter::bt( const irr::core::vector3df &irrVec )
{
    return btVector3( irrVec.X, irrVec.Y, irrVec.Z );
}
