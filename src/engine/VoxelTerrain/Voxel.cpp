#include "VoxelTerrain/Voxel.h"

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

Voxel::Voxel()
    : fillLevel(2),
      triangleUp(true)
{
    for(int x = 0; x < 4; ++x)
    {
        neighbours[x] = 0;
    }
}


Voxel::Voxel(int index, int height, vector3df position, vector3df vertices0,
             vector3df vertices1, vector3df vertices2, short fillLevel,
             Voxel::E_VOXEL_TYPE type, bool triangleUp)
    : index(index),
      height(height),
      position(position),
      vertices({vertices0, vertices1, vertices2}),
      fillLevel(fillLevel),
      type(type),
      triangleUp(triangleUp)
{
    for(int x = 0; x < 4; ++x)
    {
        neighbours[x] = 0;
    }
}
