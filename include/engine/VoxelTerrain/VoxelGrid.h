#ifndef VOXELGRID_H
#define VOXELGRID_H

#include "../EngineTypedefs.h"
#include "VoxelTerrain/Voxel.h"

class VoxelGrid
{
public:
    VoxelGrid(IrrlichtDevicePtr device, BulletWorldPtr bulletWorld,
              float length, int subdiv, int depth);

    ~VoxelGrid();

    EntityPtr getEntity()
    {
        return mEntity;
    }

private:
    IrrlichtDevicePtr mDevice;
    BulletWorldPtr mBulletWorld;
    std::vector<boost::shared_ptr<Voxel>> mVoxels;
    irr::scene::ISceneNode *mSceneNode;
    irr::scene::SMesh *mMesh;
    irr::scene::SMeshBuffer *mMeshBuffer;
    EntityPtr mEntity;
};

#endif // VOXELGRID_H
