#ifndef VOXEL_H
#define VOXEL_H

#include "../EngineTypedefs.h"
#include "game/Explore.h"

struct Voxel
{
    enum E_VOXEL_TYPE
    {
        EVT_AIR = 0,
        EVT_GROUND
    };

    enum E_NEIGHBOUR
    {
        EN_LEFT = 0,
        EN_RIGHT,
        EN_TOP,
        EN_ABOVE,
        EN_BELOW
    };

    Voxel();

    Voxel(int index,
            int height,
            irr::core::vector3df position,
            irr::core::vector3df vertices0,
            irr::core::vector3df vertices1,
            irr::core::vector3df vertices2,
            short fillLevel,
            E_VOXEL_TYPE type,
            bool triangleUp);

    int index, height;
    irr::core::vector3df position;
    irr::core::vector3df vertices[3];
    Voxel* neighbours[5];
    short fillLevel;
    E_VOXEL_TYPE type;
    bool triangleUp;
};

#endif // VOXEL_H
