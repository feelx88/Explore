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

class VoxelGrid
{
public:
    VoxelGrid(ExplorePtr explore, float length, int subdiv, int depth)
        : mExplore(explore)
    {
        /*int numVoxel = pow(2.f, 2 * subdiv);
        mVoxels = new int*[numVoxel];
        for(int i = 0; i < depth; ++i)
        {
            mVoxels[i] = new int[depth];
            for(int j = 0; j < numVoxel; ++j)
            {
                mVoxels[i][j] = 10;
            }
        }*/

        IrrlichtDevicePtr device = explore->getIrrlichtDevice();
        ISceneManagerPtr mgr = device->getSceneManager();
        BulletWorldPtr world = explore->getBulletWorld();

        vector3df a(0.f, 0.f, length);
        vector3df b(0.75f * length, 0.f, -0.25f * length);
        vector3df c(-0.75f * length, 0.f, -0.25f * length);

        mMeshBuffer = new SMeshBuffer();

        std::vector<vector3df> verts;
        std::vector<int> indices;

        calculateSubdividedTriangles(
                    triangle3df(a, b, c), subdiv, verts, indices);

        boost::random::mt19937 rand;
        boost::random::uniform_real_distribution<float> dist(-1.f, 1.f);

        for(unsigned int x = 0; x < verts.size(); x += 3)
        {
            S3DVertex v1, v2, v3;
            v1.Pos = verts.at(x) + vector3df(0.f, dist(rand), 0.f);
            v1.TCoords = vector2df(0.f, 0.f);

            v2.Pos = verts.at(x + 1) + vector3df(0.f, dist(rand), 0.f);
            v2.TCoords = vector2df(0.f, 1.f);

            v3.Pos = verts.at(x + 2) + vector3df(0.f, dist(rand), 0.f);
            v3.TCoords = vector2df(1., 0.5f);

            vector3df normal = v1.Pos.crossProduct(v2.Pos);
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
        }

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

    void calculateSubdividedTriangles(
            triangle3df tri,
            int subdivisions,
            std::vector<vector3df> &vertexList,
            std::vector<int> &indexList)
    {
        if(subdivisions == 0)
        {
            vertexList.push_back(tri.pointA);
            vertexList.push_back(tri.pointB);
            vertexList.push_back(tri.pointC);
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
    }

    EntityPtr getEntity()
    {
        return mEntity;
    }

private:
    ExplorePtr mExplore;
    int **mVoxels;
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

    VoxelGrid grid1(mExplore, 128.f, 7, 0);
    VoxelGrid grid2(mExplore, 128.f, 7, 0);
    VoxelGrid grid3(mExplore, 128.f, 7, 0);
    grid1.getEntity()->setPosition(vector3df(0,-2,0));
    grid2.getEntity()->setPosition(vector3df(2*96,-2,0));
    grid3.getEntity()->setPosition(vector3df(96,-2,96));
    grid3.getEntity()->setRotation(vector3df(0,180,0));

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
