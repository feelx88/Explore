/*
    Copyright 2014 Felix Müller.

    This file is part of %PROJECTNAME%.

    %PROJECTNAME% is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    %PROJECTNAME% is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with %PROJECTNAME%.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <engine/VoxelTerrain/VoxelSector.h>

using namespace irr;
using namespace core;
using namespace video;
using namespace scene;

VoxelSector::VoxelSector(IrrlichtDevicePtr device, BulletWorldPtr bulletWorld)
    : mDevice(device),
      mBulletWorld(bulletWorld)
{
    mGrids.resize(6);

    for( int x = 0; x < 3; ++x)
    {
        mGrids[x].reset(new VoxelGrid(mDevice, mBulletWorld, 128.f, 7, 0));

        if(x % 2 != 0)
        {
            mGrids[x]->getEntity()->setRotation(vector3df(0.f, 180.f, 0.f));
            mGrids[x]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64,
                                                  -2.f,
                                                  64 * 0.87));
        }
        else
        {
            mGrids[x]->getEntity()->setPosition(vector3df(
                                                  -64+ x * 64,
                                                  -2.f,
                                                  64 * 0.87));
        }
    }

    for( int x = 0; x < 3; ++x)
    {
        mGrids[x + 3].reset(new VoxelGrid(mDevice, mBulletWorld, 128.f, 7, 0));

        if(x % 2 == 0)
        {
            mGrids[x + 3]->getEntity()->setRotation(vector3df(0.f, 180.f, 0.f));
            mGrids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64,
                                                  -2.f,
                                                  -64 * 0.87));
        }
        else
        {
            mGrids[x + 3]->getEntity()->setPosition(vector3df(
                                                  -64 + x * 64,
                                                  -2.f,
                                                  -64 * 0.87));
        }
    }
}
