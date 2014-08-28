#include "VoxelTerrain/VoxelGrid.h"
#include <boost/random.hpp>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

VoxelGrid::VoxelGrid(IrrlichtDevicePtr device, BulletWorldPtr bulletWorld,
                     float length, int subdiv, int depth)
    : mDevice(device),
      mBulletWorld(bulletWorld)
{
    ISceneManagerPtr sceneMgr = mDevice->getSceneManager();
    IVideoDriverPtr driver = mDevice->getVideoDriver();

    int subdivPow = std::pow(2.f, subdiv);
    int maxTris = 2 * subdivPow - 1;
    float triSize = length / subdivPow;

    vector3df offset(-length / 2.f, 0, -length / 2.f * 0.87f);

    int index = 0;

    float left = 0;

    boost::random::mt19937 rand;
    boost::random::uniform_int_distribution<> dist(0, 1);

    for(int x = 0; x < subdivPow; ++x)
    {
        int limit = (maxTris - (2 * x)) / 2;
        for(int y = 0; y <= limit; ++y)
        {
            int r1 = dist(rand);
            boost::shared_ptr<Voxel> v1(new Voxel(index++, depth + (float)r1, vector3df(),
                                                  (vector3df(y + left,0,x * 0.87) + offset) * triSize,
                                                  (vector3df(y + left + 1,0,x * 0.87) + offset) * triSize,
                                                  (vector3df(y + left + 0.5,0,x * 0.87 + 0.87) + offset) * triSize,
                                                  0 == 0 ? 2 : 1 + dist(rand), Voxel::EVT_GROUND, true));
            mVoxels.push_back(v1);

            if(y < limit)
            {
                int r2 = dist(rand);
                boost::shared_ptr<Voxel> v2(new Voxel(index++, depth + (float)r2, vector3df(),
                                                      (vector3df(y + left + 0.5,0,x * 0.87 + 0.87) + offset) * triSize,
                                                      (vector3df(y + left + 1.5,0,x * 0.87 + 0.87) + offset) * triSize,
                                                      (vector3df(y + left + 1,0,x * 0.87) + offset) * triSize,
                                                      0 == 0 ? 2 : 1 + dist(rand), Voxel::EVT_GROUND, false));
                mVoxels.push_back(v2);
            }
        }
        left += 0.5;
    }

    index = 0;

    for(int x = 0; x < subdivPow; ++x)
    {
        int limit = (maxTris - (2 * x));
        for(int y = 0; y < limit; ++y)
        {
            boost::shared_ptr<Voxel> curVoxel = mVoxels.at(index);

            if(y > 0)
            {
                curVoxel->neighbours[Voxel::EN_LEFT] = mVoxels.at(index - 1).get();
            }
            if(y < limit - 1)
            {
                curVoxel->neighbours[Voxel::EN_RIGHT] = mVoxels.at(index + 1).get();
            }
            if(curVoxel->triangleUp)
            {
                if(x > 0)
                {
                    curVoxel->neighbours[Voxel::EN_TOP] = mVoxels.at(index - (maxTris - (2 * (x - 1)))).get();
                }
            }
            else
            {
                curVoxel->neighbours[Voxel::EN_TOP] = mVoxels.at(index + (maxTris - (2 * (x + 1)))).get();
            }

            ++index;
        }
    }

    mMeshBuffer = new SMeshBuffer();

    SMeshBuffer *bufferLeft = new SMeshBuffer();
    SMeshBuffer *bufferRight = new SMeshBuffer();
    SMeshBuffer *bufferTop = new SMeshBuffer();
    int bufferIndex = 0;
    int bufferIndexLeft = 0;
    int bufferIndexRight = 0;
    int bufferIndexTop = 0;

    for(auto x : mVoxels)
    {
        vector3df height(0, x->height, 0);

        S3DVertex v1, v2, v3;
        v1.Pos = x->vertices[0];
        v1.TCoords = vector2df(0.f, 0.f);
        v1.Color = SColor(255, 255, 255, 255);

        v2.Pos = x->vertices[1] ;
        v2.TCoords = vector2df(1.f, 0.f);
        v2.Color = SColor(255, 255, 255, 255);

        v3.Pos = x->vertices[2];
        v3.TCoords = vector2df(0.5f, 1.f);
        v3.Color = SColor(255, 255, 255, 255);

        v1.Normal = v2.Normal = v3.Normal = vector3df(0,1,0);

        if(x->fillLevel == 1) //halb => schräge
        {
            if(x->neighbours[Voxel::EN_LEFT])
            {
                int lh = x->neighbours[Voxel::EN_LEFT]->height - x->height;
                if(lh == 0)
                {
                    v2.Pos.Y = -1;
                }
            }

            if(x->neighbours[Voxel::EN_RIGHT])
            {
                int rh = x->neighbours[Voxel::EN_RIGHT]->height - x->height;
                if(rh == 0)
                {
                    v1.Pos.Y = -1;
                }
            }

            if(x->neighbours[Voxel::EN_TOP])
            {
                int th = x->neighbours[Voxel::EN_TOP]->height - x->height;
                if(th == 0)
                {
                    v3.Pos.Y = -1;
                }
            }

            int sum = v1.Pos.Y + v2.Pos.Y + v3.Pos.Y;
            if(sum == -3)
            {
                v1.Pos.Y = -0.5f;
                v2.Pos.Y = -0.5f;
                v3.Pos.Y = -0.5f;
            }
            else if(sum == -2)
            {
                if(v1.Pos.Y == 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[1];
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[2];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    bufferTop->Vertices.push_back(vv1);
                    bufferTop->Vertices.push_back(vv2);
                    bufferTop->Vertices.push_back(vv3);
                    bufferLeft->Vertices.push_back(vv4);
                    bufferLeft->Vertices.push_back(vv5);
                    bufferLeft->Vertices.push_back(vv6);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                }
                else if(v2.Pos.Y == 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[0];
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[2];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    bufferTop->Vertices.push_back(vv1);
                    bufferTop->Vertices.push_back(vv2);
                    bufferTop->Vertices.push_back(vv3);
                    bufferRight->Vertices.push_back(vv4);
                    bufferRight->Vertices.push_back(vv5);
                    bufferRight->Vertices.push_back(vv6);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferTop->Indices.push_back(bufferIndexTop++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                }
                else if(v3.Pos.Y == 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[0];
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[1];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    bufferLeft->Vertices.push_back(vv1);
                    bufferLeft->Vertices.push_back(vv2);
                    bufferLeft->Vertices.push_back(vv3);
                    bufferRight->Vertices.push_back(vv4);
                    bufferRight->Vertices.push_back(vv5);
                    bufferRight->Vertices.push_back(vv6);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                    bufferLeft->Indices.push_back(bufferIndexLeft++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                    bufferRight->Indices.push_back(bufferIndexRight++);
                }
            }
            else if(sum == -1)
            {

            }
        }//else wenn voll => wand
        else
        {
            if(x->neighbours[Voxel::EN_LEFT])
            {
                int lh = x->neighbours[Voxel::EN_LEFT]->height - x->height;
                if(lh < 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[0];
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[2];
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[0];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    if(x->triangleUp)
                    {
                        bufferLeft->Vertices.push_back(vv2);
                        bufferLeft->Vertices.push_back(vv1);
                        bufferLeft->Vertices.push_back(vv3);
                        bufferLeft->Vertices.push_back(vv5);
                        bufferLeft->Vertices.push_back(vv4);
                        bufferLeft->Vertices.push_back(vv6);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                    }
                    else
                    {
                        bufferLeft->Vertices.push_back(vv1);
                        bufferLeft->Vertices.push_back(vv2);
                        bufferLeft->Vertices.push_back(vv3);
                        bufferLeft->Vertices.push_back(vv4);
                        bufferLeft->Vertices.push_back(vv5);
                        bufferLeft->Vertices.push_back(vv6);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                        bufferLeft->Indices.push_back(bufferIndexLeft++);
                    }
                }
            }

            if(x->neighbours[Voxel::EN_RIGHT])
            {
                int rh = x->neighbours[Voxel::EN_RIGHT]->height - x->height;
                if(rh < 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[1];
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[2];
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[2] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[1];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    if(x->triangleUp)
                    {
                        bufferRight->Vertices.push_back(vv2);
                        bufferRight->Vertices.push_back(vv1);
                        bufferRight->Vertices.push_back(vv3);
                        bufferRight->Vertices.push_back(vv5);
                        bufferRight->Vertices.push_back(vv4);
                        bufferRight->Vertices.push_back(vv6);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                    }
                    else
                    {
                        bufferRight->Vertices.push_back(vv1);
                        bufferRight->Vertices.push_back(vv2);
                        bufferRight->Vertices.push_back(vv3);
                        bufferRight->Vertices.push_back(vv4);
                        bufferRight->Vertices.push_back(vv5);
                        bufferRight->Vertices.push_back(vv6);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                        bufferRight->Indices.push_back(bufferIndexRight++);
                    }
                }
            }

            if(x->neighbours[Voxel::EN_TOP])
            {
                int th = x->neighbours[Voxel::EN_TOP]->height - x->height;
                if(th < 0)
                {
                    S3DVertex vv1, vv2, vv3, vv4, vv5, vv6;
                    vv1.Pos = x->vertices[0];
                    vv1.TCoords = vector2df(0.f, 0.f);
                    vv1.Color = SColor(255, 255, 255, 255);

                    vv2.Pos = x->vertices[1];
                    vv2.TCoords = vector2df(0.f, 0.f);
                    vv2.Color = SColor(255, 255, 255, 255);

                    vv3.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv3.TCoords = vector2df(0.f, 0.f);
                    vv3.Color = SColor(255, 255, 255, 255);

                    vv4.Pos = x->vertices[1] + vector3df(0,1,0);
                    vv4.TCoords = vector2df(0.f, 0.f);
                    vv4.Color = SColor(255, 255, 255, 255);

                    vv5.Pos = x->vertices[0] + vector3df(0,1,0);
                    vv5.TCoords = vector2df(0.f, 0.f);
                    vv5.Color = SColor(255, 255, 255, 255);

                    vv6.Pos = x->vertices[0];
                    vv6.TCoords = vector2df(0.f, 0.f);
                    vv6.Color = SColor(255, 255, 255, 255);

                    if(x->triangleUp)
                    {
                        bufferTop->Vertices.push_back(vv2);
                        bufferTop->Vertices.push_back(vv1);
                        bufferTop->Vertices.push_back(vv3);
                        bufferTop->Vertices.push_back(vv5);
                        bufferTop->Vertices.push_back(vv4);
                        bufferTop->Vertices.push_back(vv6);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                    }
                    else
                    {
                        bufferTop->Vertices.push_back(vv1);
                        bufferTop->Vertices.push_back(vv2);
                        bufferTop->Vertices.push_back(vv3);
                        bufferTop->Vertices.push_back(vv4);
                        bufferTop->Vertices.push_back(vv5);
                        bufferTop->Vertices.push_back(vv6);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                        bufferTop->Indices.push_back(bufferIndexTop++);
                    }
                }
            }
        }

        v1.Pos += height;
        v2.Pos += height;
        v3.Pos += height;

        if(x->triangleUp)
        {
            mMeshBuffer->Vertices.push_back(v1);
            mMeshBuffer->Vertices.push_back(v3);
            mMeshBuffer->Vertices.push_back(v2);
        }
        else
        {
            mMeshBuffer->Vertices.push_back(v1);
            mMeshBuffer->Vertices.push_back(v2);
            mMeshBuffer->Vertices.push_back(v3);
        }

        mMeshBuffer->Indices.push_back(bufferIndex++);
        mMeshBuffer->Indices.push_back(bufferIndex++);
        mMeshBuffer->Indices.push_back(bufferIndex++);
    }

    mMeshBuffer->recalculateBoundingBox();
    bufferLeft->recalculateBoundingBox();
    bufferRight->recalculateBoundingBox();
    bufferTop->recalculateBoundingBox();

    mMesh = new SMesh();
    mMesh->addMeshBuffer(mMeshBuffer);
    mMesh->addMeshBuffer(bufferLeft);
    mMesh->addMeshBuffer(bufferRight);
    mMesh->addMeshBuffer(bufferTop);
    mMesh->setDirty();
    mMesh->recalculateBoundingBox();

    mSceneNode = sceneMgr->addMeshSceneNode(mMesh, 0, -1, vector3df(0, 0, 0));
    mSceneNode->setMaterialTexture(0, driver->getTexture("data/Textures/terrain_test.png"));

    PropTreePtr props(new PropTree());
    props->put<std::string>("Entity.Body.Shape.<xmlattr>.Type", "Mesh");
    props->put<float>("Entity.Body.Mass", 0.f);

    mEntity.reset(new Entity(mDevice, mBulletWorld, props, "data", mSceneNode));
}

VoxelGrid::~VoxelGrid()
{
}
