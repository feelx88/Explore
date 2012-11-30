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

#include "VisualPlayer.h"

using namespace irr;
using namespace core;

VisualPlayer::VisualPlayer( ExplorePtr explore, IPlayerPtr parent )
    : IPlayer( explore, parent ),
      mActiveItem( -1 )
{
}

EntityPtr VisualPlayer::getEntity() const
{
    return mEntity;
}

vector3df VisualPlayer::rotateToDirection(vector3df dir) const
{
    matrix4 m = mEntity->getSceneNode()->getAbsoluteTransformation();
    m.rotateVect( dir );

    return dir;
}

vector3df VisualPlayer::getPosition() const
{
    return *mEntity->getPosition();
}

quaternion VisualPlayer::getRotation() const
{
    return *mEntity->getRotation();
}

ItemPtr VisualPlayer::getActiveItem() const
{
    if( mActiveItem < 0 )
        return ItemPtr();

    return mInventory.at( mActiveItem );
}
