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


#include <ExploreGame.h>
#include <Explore.h>
#include <players/WorldPlayer.h>
#include <players/LocalPlayer.h>
#include <engine/IrrlichtTools.h>
#include <engine/PathTools.h>
#include <engine/LoggerSingleton.h>
#include <engine/BulletSceneNodeAnimator.h>
#include <engine/Entity.h>
#include <engine/EntityContainer.h>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/random.hpp>
#include <ItemFactory.h>
#include <cmath>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;
using namespace gui;

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

    Voxel()
        : fillLevel(2),
          triangleUp(true)
    {
        for(int x = 0; x < 4; ++x)
        {
            neighbours[x] = 0;
        }
    }

    Voxel(int index,
            int height,
            vector3df position,
            vector3df vertices0,
            vector3df vertices1,
            vector3df vertices2,
            short fillLevel,
            E_VOXEL_TYPE type,
            bool triangleUp)
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

    int index, height;
    vector3df position;
    vector3df vertices[3];
    Voxel* neighbours[5];
    short fillLevel;
    E_VOXEL_TYPE type;
    bool triangleUp;
};

class VoxelGrid
{
public:
    VoxelGrid(ExplorePtr explore, float length, int subdiv, int depth)
        : mExplore(explore)
    {
        IrrlichtDevicePtr device = explore->getIrrlichtDevice();
        ISceneManagerPtr mgr = device->getSceneManager();
        BulletWorldPtr world = explore->getBulletWorld();

        vector3df a(0.f, 0.f, length);
        vector3df b(0.75f * length, 0.f, -0.25f * length);
        vector3df c(-0.75f * length, 0.f, -0.25f * length);

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

        mSceneNode = mgr->addMeshSceneNode(mMesh, 0, -1, vector3df(0, 0, 0));
        mSceneNode->setMaterialTexture(0, mExplore->getIrrlichtDevice()->getVideoDriver()->getTexture("data/Textures/terrain_test.png"));

        PropTreePtr props(new PropTree());
        props->put<std::string>("Entity.Body.Shape.<xmlattr>.Type", "Mesh");
        props->put<float>("Entity.Body.Mass", 0.f);

        mEntity.reset(new Entity(device, world, props, "data", mSceneNode));
    }

    ~VoxelGrid()
    {
    }

    EntityPtr getEntity()
    {
        return mEntity;
    }

private:
    ExplorePtr mExplore;
    std::vector<boost::shared_ptr<Voxel>> mVoxels;
    ISceneNode *mSceneNode;
    SMesh *mMesh;
    SMeshBuffer *mMeshBuffer;
    EntityPtr mEntity;
};

ExploreGame::ExploreGame( ExplorePtr explore )
    : mExplore( explore ),
      mDevice( explore->getIrrlichtDevice() ),
      mVideoDriver( mDevice->getVideoDriver() ),
      mSceneManager( mDevice->getSceneManager() ),
      mGUI( mDevice->getGUIEnvironment() ),
      mEventReceiver( explore->getEventReceiver() ),
      mBulletWorld( explore->getBulletWorld() ),
      mBulletDebugDraw( false )
{
}

E_GAME_STATE ExploreGame::run()
{
    bool running = true;

    mSceneManager->addLightSceneNode( 0, vector3df( 0.f, 50.f, 0.f ),
                                      SColorf( 1, 1, 1 ), 1000.f );

    EntityContainerPtr level( new EntityContainer(
                                  mDevice, mBulletWorld,
                                  PathTools::getAbsolutePath( "TestMap2" ) ) );

    vector3df spawnPos =
            level->getProperties()->get( "Spawn.Position", vector3df() );

    if( !mWorldPlayer )
    {
        _LOG( "Creating new World" );
        mWorldPlayer.reset( new WorldPlayer( mExplore ) );
    }

    VisualPlayerPtr p;

    p.reset( new LocalPlayer( mExplore, mWorldPlayer ),
             specialDeleters::NullDeleter() );
    mWorldPlayer->setLocalPlayer( p );

    ItemFactory::create( mExplore, p, "de.feelx88.SimpleForceGun" );
    ItemFactory::create( mExplore, p, "de.feelx88.SimpleBlockSpawner" );
    ItemFactory::create( mExplore, p, "de.feelx88.SimpleGun" );
    ItemFactory::create( mExplore, p, "de.feelx88.SuzanneSpawner" );

    p->getEntity()->setPosition( spawnPos );

    mBulletWorld->setGravity( btVector3( 0.f, -10.f, 0.f ) );

    std::vector<boost::shared_ptr<VoxelGrid> > grids;
    grids.resize(6);

    for( int x = 0; x < 3; ++x)
    {
        grids[x].reset(new VoxelGrid(mExplore, 128.f, 7, 0));

        if(x % 2 != 0)
        {
            grids[x]->getEntity()->setRotation(vector3df(0.f, 180.f, 0.f));
            grids[x]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64/*-96.f + x * 96.f*/,
                                                  -2.f,
                                                  64 * 0.87/*80.f + 96.f*/));
        }
        else
        {
            grids[x]->getEntity()->setPosition(vector3df(
                                                  -64+ x * 64/*-96.f + x * 96.f*/,
                                                  -2.f,
                                                  64 * 0.87/*80.f*/));
        }
    }

    for( int x = 0; x < 3; ++x)
    {
        grids[x + 3].reset(new VoxelGrid(mExplore, 128.f, 7, 0));

        if(x % 2 == 0)
        {
            grids[x + 3]->getEntity()->setRotation(vector3df(0.f, 180.f, 0.f));
            grids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64/*-96.f + x * 96.f*/,
                                                  -2.f,
                                                  -64 * 0.87/*-80.f + 96.f*/));
        }
        else
        {
            grids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64 /*-96.f + x * 96.f*/,
                                                  -2.f,
                                                  -64 * 0.87/*-80.f*/));
        }
    }

    btClock clock;

    while( running && mDevice->run() )
    {
        mBulletWorld->stepSimulation(
                    float( clock.getTimeMilliseconds() ) / 1000.f, 50 );
        clock.reset();

        mVideoDriver->beginScene( true, true, SColor( 255, 0, 0, 0 ) );
        mSceneManager->drawAll();
        mGUI->drawAll();

        if( mEventReceiver->keyClicked( irr::KEY_ESCAPE ) )
        {
            _LOG( "Escape key pressed" );
            mExplore->setGameState( EGS_MAIN_MENU );
            running = false;
        }

        mWorldPlayer->update();

        if( mBulletDebugDraw )
            mBulletWorld->debugDrawWorld();

        mVideoDriver->endScene();

        mDevice->setWindowCaption( stringw( mVideoDriver->getFPS() ).c_str() );
        mExplore->getIOService()->poll();
    }

    mExplore->getExploreServer()->setServerMode( false );
    mExplore->getExploreServer()->disconnect();

    mWorldPlayer.reset();

    mExplore->setGameState( EGS_MAIN_MENU );

    return EGS_MAIN_MENU;//TODO: change signature
}

void ExploreGame::setBulletDebugDraw( bool enabled )
{
    mBulletDebugDraw = enabled;
}

WorldPlayerPtr ExploreGame::getWorldPlayer()
{
    return mWorldPlayer;
}

void ExploreGame::setWorldPlyer(WorldPlayerPtr world)
{
    //Ensure that mWorldPlayer cannot be overwritten
    if( !mWorldPlayer )
        mWorldPlayer = world;
}
