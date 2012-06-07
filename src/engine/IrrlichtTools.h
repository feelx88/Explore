#ifndef IRRLICHTTOOLS_H
#define IRRLICHTTOOLS_H

#include "EngineTypedefs.h"
#include <string>
#include <sstream>

class IrrlichtTools
{
public:
    static IrrlichtDevicePtr createIrrlichtDevicePtr( irr::IrrlichtDevice *device );

    static irr::gui::IGUIButton *guiCenterFittingButton(
            irr::gui::IGUIEnvironment *env, const std::wstring &label, int x, int y );
};

namespace irr
{
namespace core
{

template <typename T>
static std::ostream &operator<<( std::ostream &stream,
                          const irr::core::vector3d<T> &in )
{
    stream << in.X << "," << in.Y << "," << in.Z;
    return stream;
}

template <typename T>
static std::istream &operator>>( std::istream &stream,
                          irr::core::vector3d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y >> tmp >> out.Z;
    return stream;
}

template <typename T>
static std::ostream &operator<<( std::ostream &stream,
                          const irr::core::vector2d<T> &in )
{
    stream << in.X << "," << in.Y;
    return stream;
}

template <typename T>
static std::istream &operator>>( std::istream &stream,
                          irr::core::vector2d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y;
    return stream;
}

}

namespace video
{

static std::ostream &operator<<( std::ostream &stream,
                          const irr::video::SColor &in )
{
    stream << in.getAlpha() << ","
           << in.getRed() << ","
           << in.getGreen() << ","
           << in.getBlue();
    return stream;
}

static std::istream &operator>>( std::istream &stream,
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
