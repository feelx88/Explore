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
std::ostream &operator<<( std::ostream &stream,
                          const irr::core::vector3d<T> &in )
{
    stream << in.X << "," << in.Y << "," << in.Z;
    return stream;
}

template <typename T>
std::istream &operator>>( std::istream &stream,
                          irr::core::vector3d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y >> tmp >> out.Z;
    return stream;
}

}
}

#endif // IRRLICHTTOOLS_H
