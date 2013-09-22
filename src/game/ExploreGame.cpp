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
    VoxelGrid(ISceneManagerPtr mgr, float length, int subdiv, int depth)
        : mManager(mgr)
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

        vector3df a(0.f, 0.f, length);
        vector3df b(0.75f * length, 0.f, -0.25f * length);
        vector3df c(-0.75f * length, 0.f, -0.25f * length);

        mMeshBuffer = new SMeshBuffer();

        std::vector<triangle3df> tris = calculateSubdividedTriangles(
                    triangle3df(a, b, c), subdiv);

        for(unsigned int x = 0; x < tris.size(); ++x)
        {
            S3DVertex v1, v2, v3;
            v1.Pos = tris.at(x).pointA;
            v1.Normal = vector3df(0,0,1);
            v1.Color = SColor(255,255,0,0);
            v2.Pos = tris.at(x).pointB;
            v2.Normal = vector3df(0,0,1);
            v2.Color = SColor(255,0,255,0);
            v3.Pos = tris.at(x).pointC;
            v3.Normal = vector3df(0,0,1);
            v3.Color = SColor(255,0,0,255);

            mMeshBuffer->Vertices.push_back(v1);
            mMeshBuffer->Vertices.push_back(v2);
            mMeshBuffer->Vertices.push_back(v3);

            mMeshBuffer->Indices.push_back(mMeshBuffer->Indices.size());
            mMeshBuffer->Indices.push_back(mMeshBuffer->Indices.size());
            mMeshBuffer->Indices.push_back(mMeshBuffer->Indices.size());
        }

        mMeshBuffer->recalculateBoundingBox();

        mMesh = new SMesh();
        mMesh->addMeshBuffer(mMeshBuffer);
        mMesh->setDirty();
        mMesh->recalculateBoundingBox();

        mSceneNode = mgr->addMeshSceneNode(mMesh, 0, -1, vector3df(0, 0, 0));
        mSceneNode->setMaterialFlag(EMF_LIGHTING, false);
    }

    ~VoxelGrid()
    {
        //delete[] mVoxels;
    }

    std::vector<triangle3df> calculateSubdividedTriangles(
            triangle3df tri,
            int subdivisions)
    {
        std::vector<triangle3df> triList;
        calculateSubdividedTrianglesHelper(tri, subdivisions, triList);
        return triList;
    }

    void calculateSubdividedTrianglesHelper(
            triangle3df tri,
            int subdivisions,
            std::vector<triangle3df> &tris)
    {
        if(subdivisions > 0)
        {
            triangle3df centerTri;

            centerTri.pointA = ( tri.pointC - tri.pointA ) / 2.f + tri.pointA;
            centerTri.pointB = ( tri.pointB - tri.pointA ) / 2.f + tri.pointA;
            centerTri.pointC = ( tri.pointC - tri.pointB ) / 2.f + tri.pointB;

            triangle3df calcTri;

            //top
            calcTri.pointA = tri.pointA;
            calcTri.pointB = centerTri.pointB;
            calcTri.pointC = centerTri.pointA;
            calculateSubdividedTrianglesHelper(calcTri, subdivisions - 1, tris);

            //left
            calcTri.pointA = centerTri.pointA;
            calcTri.pointB = centerTri.pointC;
            calcTri.pointC = tri.pointC;
            calculateSubdividedTrianglesHelper(calcTri, subdivisions - 1, tris);

            //right
            calcTri.pointA = centerTri.pointB;
            calcTri.pointB = tri.pointB;
            calcTri.pointC = centerTri.pointC;
            calculateSubdividedTrianglesHelper(calcTri, subdivisions - 1, tris);

            //center
            calcTri = triangle3df(centerTri);
            calculateSubdividedTrianglesHelper(calcTri, subdivisions - 1, tris);
        }
        else
        {
            tris.push_back(tri);
        }
    }

    void update()
    {

    }

//private:
    int **mVoxels;
    ISceneManagerPtr mManager;
    ISceneNode *mSceneNode;
    SMesh *mMesh;
    SMeshBuffer *mMeshBuffer;
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

    VoxelGrid grid1(mSceneManager, 100.f, 7, 0);
    VoxelGrid grid2(mSceneManager, 100.f, 7, 0);
    VoxelGrid grid3(mSceneManager, 100.f, 7, 0);
    grid1.mSceneNode->setPosition(vector3df(0,-50,0));
    grid2.mSceneNode->setPosition(vector3df(2*75,-50,0));
    grid3.mSceneNode->setPosition(vector3df(75,-50,75));
    grid3.mSceneNode->setRotation(vector3df(0,180,0));
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
