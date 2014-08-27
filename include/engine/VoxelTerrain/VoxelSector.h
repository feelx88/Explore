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

#ifndef VOXELSECTOR_H
#define VOXELSECTOR_H

#include <EngineTypedefs.h>
#include <VoxelTerrain/VoxelGrid.h>
#include <Explore.h>

class VoxelSector
{
public:
    VoxelSector(ExplorePtr explore);

private:
    std::vector<boost::shared_ptr<VoxelGrid> > mGrids;
    ExplorePtr mExplore;
};

#endif // VOXELSECTOR_H
