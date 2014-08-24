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

std::vector<Voxel*> v = {
    new Voxel( 0, 10, vector3df(), vector3df(0,0,0),vector3df(1,0,0),vector3df(0.5,0,1), 2, Voxel::EVT_GROUND, true),
    new Voxel( 1, 10, vector3df(), vector3df(0.5,0,1),vector3df(1.5,0,1),vector3df(1,0,0), 2, Voxel::EVT_GROUND, false),
    new Voxel( 2, 10, vector3df(), vector3df(1,0,0),vector3df(2,0,0),vector3df(1.5,0,1), 2, Voxel::EVT_GROUND, true),
    new Voxel( 3, 10, vector3df(), vector3df(1.5,0,1),vector3df(2.5,0,1),vector3df(2,0,0), 2, Voxel::EVT_GROUND, false),
    new Voxel( 4, 10, vector3df(), vector3df(2,0,0),vector3df(3,0,0),vector3df(2.5,0,1), 2, Voxel::EVT_GROUND, true),
    new Voxel( 5, 10, vector3df(), vector3df(2.5,0,1),vector3df(3.5,0,1),vector3df(3,0,0), 2, Voxel::EVT_GROUND, false),
    new Voxel( 6, 10, vector3df(), vector3df(3,0,0),vector3df(4,0,0),vector3df(3.5,0,1), 2, Voxel::EVT_GROUND, true),

    new Voxel( 7, 11, vector3df(), vector3df(0.5,0,1),vector3df(1.5,0,1),vector3df(1,0,2), 1, Voxel::EVT_GROUND, true),
    new Voxel( 8, 11, vector3df(), vector3df(1,0,2),vector3df(2,0,2),vector3df(1.5,0,1), 2, Voxel::EVT_GROUND, false),
    new Voxel( 9, 11, vector3df(), vector3df(1.5,0,1),vector3df(2.5,0,1),vector3df(2,0,2), 1, Voxel::EVT_GROUND, true),
    new Voxel(10, 10, vector3df(), vector3df(2,0,2),vector3df(3,0,2),vector3df(2.5,0,1), 2, Voxel::EVT_GROUND, false),
    new Voxel(11, 10, vector3df(), vector3df(2.5,0,1),vector3df(3.5,0,1),vector3df(3,0,2), 2, Voxel::EVT_GROUND, true),

    new Voxel(12, 11, vector3df(), vector3df(1,0,2),vector3df(2,0,2),vector3df(1.5,0,3), 1, Voxel::EVT_GROUND, true),
    new Voxel(13, 11, vector3df(), vector3df(1.5,0,3),vector3df(2.5,0,3),vector3df(2,0,2), 2, Voxel::EVT_GROUND, false),
    new Voxel(14, 10, vector3df(), vector3df(2,0,2),vector3df(3,0,2),vector3df(2.5,0,3), 2, Voxel::EVT_GROUND, true),

    new Voxel(15, 10, vector3df(), vector3df(1.5,0,3),vector3df(2.5,0,3),vector3df(2,0,4), 2, Voxel::EVT_GROUND, true)
};

void setV()
{
    //no
    v.at(0)->neighbours[Voxel::EN_RIGHT] = v.at(1);
    //no

    v.at(1)->neighbours[Voxel::EN_LEFT] = v.at(0);
    v.at(1)->neighbours[Voxel::EN_RIGHT] = v.at(2);
    v.at(1)->neighbours[Voxel::EN_TOP] = v.at(7);

    v.at(2)->neighbours[Voxel::EN_LEFT] = v.at(1);
    v.at(2)->neighbours[Voxel::EN_RIGHT] = v.at(3);
    //no

    v.at(3)->neighbours[Voxel::EN_LEFT] = v.at(2);
    v.at(3)->neighbours[Voxel::EN_RIGHT] = v.at(4);
    v.at(3)->neighbours[Voxel::EN_TOP] = v.at(9);

    v.at(4)->neighbours[Voxel::EN_LEFT] = v.at(3);
    v.at(4)->neighbours[Voxel::EN_RIGHT] = v.at(5);
    //no

    v.at(5)->neighbours[Voxel::EN_LEFT] = v.at(4);
    v.at(5)->neighbours[Voxel::EN_RIGHT] = v.at(6);
    v.at(5)->neighbours[Voxel::EN_TOP] = v.at(11);

    v.at(6)->neighbours[Voxel::EN_LEFT] = v.at(5);
    //no
    //no

    //no
    v.at(7)->neighbours[Voxel::EN_RIGHT] = v.at(8);
    v.at(7)->neighbours[Voxel::EN_TOP] = v.at(1);

    v.at(8)->neighbours[Voxel::EN_LEFT] = v.at(7);
    v.at(8)->neighbours[Voxel::EN_RIGHT] = v.at(9);
    v.at(8)->neighbours[Voxel::EN_TOP] = v.at(12);

    v.at(9)->neighbours[Voxel::EN_LEFT] = v.at(8);
    v.at(9)->neighbours[Voxel::EN_RIGHT] = v.at(10);
    v.at(9)->neighbours[Voxel::EN_TOP] = v.at(3);

    v.at(10)->neighbours[Voxel::EN_LEFT] = v.at(9);
    v.at(10)->neighbours[Voxel::EN_RIGHT] = v.at(11);
    v.at(10)->neighbours[Voxel::EN_TOP] = v.at(14);

    v.at(11)->neighbours[Voxel::EN_LEFT] = v.at(10);
    //no
    v.at(11)->neighbours[Voxel::EN_TOP] = v.at(5);

    //no
    v.at(12)->neighbours[Voxel::EN_RIGHT] = v.at(13);
    v.at(12)->neighbours[Voxel::EN_TOP] = v.at(8);

    v.at(13)->neighbours[Voxel::EN_LEFT] = v.at(12);
    v.at(13)->neighbours[Voxel::EN_RIGHT] = v.at(14);
    v.at(13)->neighbours[Voxel::EN_TOP] = v.at(15);

    v.at(14)->neighbours[Voxel::EN_LEFT] = v.at(13);
    //no
    v.at(14)->neighbours[Voxel::EN_TOP] = v.at(10);

    //no
    //no
    v.at(15)->neighbours[Voxel::EN_TOP] = v.at(13);
}


class VoxelGrid
{
public:
    VoxelGrid(ExplorePtr explore, float length, int subdiv, int depth)
        : mExplore(explore)
    {
        int numVoxel = pow(2.f, 2 * subdiv);
        /*mVoxels = new int*[numVoxel];
        for(int i = 0; i < depth; ++i)
        {
            mVoxels[i] = new int[depth];
            for(int j = 0; j < numVoxel; ++j)
            {
                mVoxels[i][j] = 10;
            }
        }*/

        mVoxels = new Voxel*[1];
        mVoxels[0] = new Voxel[numVoxel];

        IrrlichtDevicePtr device = explore->getIrrlichtDevice();
        ISceneManagerPtr mgr = device->getSceneManager();
        BulletWorldPtr world = explore->getBulletWorld();

        vector3df a(0.f, 0.f, length);
        vector3df b(0.75f * length, 0.f, -0.25f * length);
        vector3df c(-0.75f * length, 0.f, -0.25f * length);

        mMeshBuffer = new SMeshBuffer();
        int counter = 0;

        setV();

        for(auto x : v)
        {
            vector3df height(0, x->height, 0);

            S3DVertex v1, v2, v3;
            v1.Pos = x->vertices[0] * 2;
            v1.TCoords = vector2df(0.f, 0.f);
            v1.Color = SColor(255, 255, 255, 255);

            v2.Pos = x->vertices[1] * 2;
            v2.TCoords = vector2df(1.f, 0.f);
            v2.Color = SColor(255, 255, 255, 255);

            v3.Pos = x->vertices[2] * 2;
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
                        //v1.Pos.Y = -1;
                        //v3.Pos.Y = -1;
                        v2.Pos.Y = -1;
                    }
                }

                if(x->neighbours[Voxel::EN_RIGHT])
                {
                    int rh = x->neighbours[Voxel::EN_RIGHT]->height - x->height;
                    if(rh == 0)
                    {
                        //v2.Pos.Y = -1;
                        //v3.Pos.Y = -1;
                        v1.Pos.Y = -1;
                    }
                }

                if(x->neighbours[Voxel::EN_TOP])
                {
                    int th = x->neighbours[Voxel::EN_TOP]->height - x->height;
                    if(th == 0)
                    {
                        //v1.Pos.Y = -1;
                        //v2.Pos.Y = -1;
                        v3.Pos.Y = -1;
                    }
                }
            }//else wenn voll => wand
            else
            {
                if(x->neighbours[Voxel::EN_LEFT])
                {
                    int lh = x->neighbours[Voxel::EN_LEFT]->height - x->height;
                    if(lh > 0)
                    {
                        //wand
                    }
                }

                if(x->neighbours[Voxel::EN_RIGHT])
                {
                    int rh = x->neighbours[Voxel::EN_RIGHT]->height - x->height;
                    if(rh > 0)
                    {
                        //wand
                    }
                }

                if(x->neighbours[Voxel::EN_TOP])
                {
                    int th = x->neighbours[Voxel::EN_TOP]->height - x->height;
                    if(th > 0)
                    {
                        //wand
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

            mMeshBuffer->Indices.push_back(counter++);
            mMeshBuffer->Indices.push_back(counter++);
            mMeshBuffer->Indices.push_back(counter++);
        }

        /*std::vector<vector3df> verts;
        std::vector<int> indices;

        calculateSubdividedTriangles(
                    triangle3df(a, b, c), subdiv, verts, indices);

        boost::random::mt19937 rand;
        boost::random::uniform_int_distribution<> dist(0, 1);

        for(unsigned int x = 0; x < verts.size(); x += 3)
        {
            S3DVertex v1, v2, v3;
            v1.Pos = verts.at(x) + vector3df(0.f, (float)dist(rand), 0.f);
            v1.TCoords = vector2df(0.f, 0.f);
            v1.Color = SColor(255, 255, 255, 255);

            v2.Pos = verts.at(x + 1) + vector3df(0.f, (float)dist(rand), 0.f);
            v2.TCoords = vector2df(1.f, 0.f);
            v2.Color = SColor(255, 255, 255, 255);

            v3.Pos = verts.at(x + 2) + vector3df(0.f, (float)dist(rand), 0.f);
            v3.TCoords = vector2df(0.5f, 1.f);
            v3.Color = SColor(255, 255, 255, 255);

            vector3df normal = (v1.Pos - v3.Pos).crossProduct((v1.Pos - v2.Pos));
            normal.normalize();

            v1.Normal = normal;
            v2.Normal = normal;
            v3.Normal = normal;

            mMeshBuffer->Vertices.push_back(v1);
            mMeshBuffer->Vertices.push_back(v2);
            mMeshBuffer->Vertices.push_back(v3);
        }

        for(unsigned int x = 0; x < indices.size(); ++x)
        {
            mMeshBuffer->Indices.push_back(indices.at(x));
        }*/

        mMeshBuffer->recalculateBoundingBox();

        mMesh = new SMesh();
        mMesh->addMeshBuffer(mMeshBuffer);
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
        //delete[] mVoxels;
    }

    /*void calculateSubdividedTriangles(
            triangle3df tri,
            int subdivisions,
            std::vector<vector3df> &vertexList,
            std::vector<int> &indexList)
    {
        if(subdivisions == 0)
        {
            vertexList.push_back(tri.pointA);
            vertexList.push_back(tri.pointC);
            vertexList.push_back(tri.pointB);
        }

        int maxVerts = (int)pow(2.f, subdivisions) + 1;
        float maxSizeX = (tri.pointB - tri.pointC).getLength();
        float maxSizeY = (tri.pointA - (tri.pointB - tri.pointC) / 2.f ).getLength();

        vector3df startPoint = tri.pointC;

        float divX = maxSizeX / (float)(maxVerts - 1);
        float divY = maxSizeY / (float)(maxVerts - 1);

        int vertCount = (maxVerts * maxVerts + maxVerts) / 2; //gauss
        int num = 0;

        for(int x = maxVerts; x > 0; --x)
        {
            for(int y = 0; y < x; ++y)
            {
                vertexList.push_back(vector3df(
                                         startPoint.X + (float)y * divX,
                                         0.f,
                                         startPoint.Z));

                if(y < x - 1)
                {
                    mVoxels[0][num].fillLevel = 1;
                    mVoxels[0][num].position = vector3df;
                    mVoxels[0][num].triangleUp = y % 2 == 0;
                    mVoxels[0][num].type = Voxel::EVT_GROUND;

                    indexList.push_back(num + 1);
                    indexList.push_back(num);
                    indexList.push_back(num + x);
                    if(num - x > 0)
                    {
                        indexList.push_back(num);
                        indexList.push_back(num + 1);
                        indexList.push_back(num - x);
                    }
                }
                num++;
            }

            startPoint.X += divX / 2.f;
            startPoint.Z += divY;
        }
    }*/

    EntityPtr getEntity()
    {
        return mEntity;
    }

private:
    ExplorePtr mExplore;
    Voxel **mVoxels;
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
                                                  -96.f + x * 96.f,
                                                  -2.f,
                                                  80.f + 96.f));
        }
        else
        {
            grids[x]->getEntity()->setPosition(vector3df(
                                                  -96.f + x * 96.f,
                                                  -2.f,
                                                  80.f));
        }
    }

    for( int x = 0; x < 3; ++x)
    {
        grids[x + 3].reset(new VoxelGrid(mExplore, 128.f, 7, 0));

        if(x % 2 == 0)
        {
            grids[x + 3]->getEntity()->setRotation(vector3df(0.f, 180.f, 0.f));
            grids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -96.f + x * 96.f,
                                                  -2.f,
                                                  -80.f + 96.f));
        }
        else
        {
            grids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -96.f + x * 96.f,
                                                  -2.f,
                                                  -80.f));
        }
    }

    grids[0]->getEntity()->setPosition(vector3df(0,-9,0));

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
