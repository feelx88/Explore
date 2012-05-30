#include "QuaternionConverter.h"

btQuaternion QuaternionConverter::bt( const irr::core::quaternion &irr )
{
    return btQuaternion( -irr.X, -irr.Y, -irr.Z, irr.W );
}

irr::core::quaternion QuaternionConverter::irr( const btQuaternion &bt )
{
    return irr::core::quaternion( -bt.getX(), -bt.getY(), -bt.getZ(), bt.getW() );
}
