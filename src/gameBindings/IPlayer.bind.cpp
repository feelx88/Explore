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

#include <engine/PythonBinder.h>
#include "../game/IPlayer.h"

class IPlayerWrapper : public IPlayer, boost::python::wrapper<IPlayer>
{
public:
    virtual void update()
    {
        this->get_override( "update" )();
    }

    virtual irr::core::vector3df rotateToDirection(
            irr::core::vector3df dir = irr::core::vector3df( 0.f, 0.f, 1.f ) ) const
    {
        return this->get_override( "rotateToDirection" )( dir );
    }

    virtual irr::core::vector3df getPosition() const
    {
        return this->get_override( "getPosition" )();
    }

    virtual irr::core::quaternion getRotation() const
    {
        return this->get_override( "getRotation" )();
    }
};

PYTHONBINDER_REGISTER_MODULE( IPlayer )
{
    using namespace boost::python;
    class_<IPlayer, boost::noncopyable>( "IPlayer", no_init )
        .def( "removeOwnedItem", &IPlayer::removeOwnedItem );
}
