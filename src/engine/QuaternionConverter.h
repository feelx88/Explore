#ifndef QUATERNIONCONVERTER_H
#define QUATERNIONCONVERTER_H

#include "EngineTypedefs.h"

class QuaternionConverter
{
public:
    static btQuaternion bt( const irr::core::quaternion &irr );
    static irr::core::quaternion irr( const btQuaternion &bt );
};

#endif // QUATERNIONCONVERTER_H
