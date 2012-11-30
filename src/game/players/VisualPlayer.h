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

#ifndef VISUALPLAYER_H
#define VISUALPLAYER_H

#include "../IPlayer.h"

class VisualPlayer : public IPlayer
{
public:
    VisualPlayer( ExplorePtr explore, IPlayerPtr parent );

    EntityPtr getEntity() const;

    irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const;
    irr::core::vector3df getPosition() const;
    irr::core::quaternion getRotation() const;

    ItemPtr getActiveItem() const;

protected:

    std::vector<ItemPtr> mInventory;
    int mActiveItem;

    EntityPtr mEntity;
};

#endif // VISUALPLAYER_H
