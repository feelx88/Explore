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

#ifndef WORLDPLAYER_H
#define WORLDPLAYER_H

#include <IPlayer.h>

class WorldPlayer;
typedef boost::shared_ptr<WorldPlayer> WorldPlayerPtr;

class APIEXPORT WorldPlayer : public IPlayer
{
public:
    WorldPlayer( ExplorePtr explore );

    void update();

    void setLocalPlayer( IPlayerPtr player );
    IPlayerPtr getLocalPlayer() const;

    irr::core::vector3df rotateToDirection( irr::core::vector3df dir ) const;
    irr::core::vector3df getPosition() const;
    irr::core::quaternion getRotation() const;

    virtual E_PLAYER_TYPE getType() const;

private:
    IPlayerWeakPtr mLocalPlayer;
};

#endif // WORLDPLAYER_H
