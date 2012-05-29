#ifndef VECTORCONVERTER_H
#define VECTORCONVERTER_H

#include "EngineTypedefs.h"

struct VectorConverter
{
    static irr::core::vector3df irr( const btVector3 &btVec );
    static btVector3 bt( const irr::core::vector3df &irrVec );
};

#endif // VECTORCONVERTER_H
