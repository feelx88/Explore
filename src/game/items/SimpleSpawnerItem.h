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

#ifndef SIMPLESPAWNERITEM_H
#define SIMPLESPAWNERITEM_H

#include "../Item.h"

class SimpleSpawnerItem : public Item
{
public:
    SimpleSpawnerItem( ExplorePtr explore, PlayerPtr owner,
                       PropTreePtr properties, const std::string &basePath );

    void startAction( E_ITEM_ACTION actionID );

private:
    void spawn( bool zeroMass );
    void update();
    irr::core::vector3df calculateHalfExtentsFromItem( PropTreePtr props );

    std::vector<std::string> mSpawnableItems;
    std::vector<irr::core::vector3df> mItemHalfExtents;

    std::vector<Item*> mSpawnedItems;

    int mCurItem;
    irr::core::vector3df mSpawnPoint;
    bool mValidSpawnPoint;

    static int sRegisterDummy;
};

#endif // SIMPLESPAWNERITEM_H
