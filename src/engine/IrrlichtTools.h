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


#ifndef IRRLICHTTOOLS_H
#define IRRLICHTTOOLS_H

#include "EngineTypedefs.h"
#include <string>
#include <sstream>

class APIEXPORT IrrlichtTools
{
public:
    static IrrlichtDevicePtr createIrrlichtDevicePtr( irr::IrrlichtDevice *device );

    static irr::gui::IGUIButton *guiCenterFittingButton(
            irr::gui::IGUIEnvironment *env, const std::wstring &label, int x, int y );

    static void guiCenterElement( IGUIElementPtr element , int x, int y );
};

namespace irr
{
namespace core
{

template <typename T>
static inline std::ostream &operator<<( std::ostream &stream,
                          const irr::core::vector3d<T> &in )
{
    stream << in.X << "," << in.Y << "," << in.Z;
    return stream;
}

template <typename T>
static inline std::istream &operator>>( std::istream &stream,
                          irr::core::vector3d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y >> tmp >> out.Z;
    return stream;
}

template <typename T>
static inline std::ostream &operator<<( std::ostream &stream,
                          const irr::core::vector2d<T> &in )
{
    stream << in.X << "," << in.Y;
    return stream;
}

template <typename T>
static inline std::istream &operator>>( std::istream &stream,
                          irr::core::vector2d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y;
    return stream;
}

}

namespace video
{

static inline std::ostream &operator<<( std::ostream &stream,
                          const irr::video::SColor &in )
{
    stream << in.getAlpha() << ","
           << in.getRed() << ","
           << in.getGreen() << ","
           << in.getBlue();
    return stream;
}

static inline std::istream &operator>>( std::istream &stream,
                          irr::video::SColor &out )
{
    char tmp;
    int a, r, g, b;
    stream >> a >> tmp >> r >> tmp >> g >> tmp >> b;
    out.set( a, r, g, b );
    return stream;
}

}
}

#endif // IRRLICHTTOOLS_H
