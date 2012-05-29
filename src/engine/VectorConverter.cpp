#include "VectorConverter.h"

irr::core::vector3df VectorConverter::irr( const btVector3 &btVec )
{
    return irr::core::vector3df( btVec.getX(), btVec.getY(), btVec.getZ() );
}


btVector3 VectorConverter::bt( const irr::core::vector3df &irrVec )
{
    return btVector3( irrVec.X, irrVec.Y, irrVec.Z );
}
