#ifndef IRRLICHTTOOLS_H
#define IRRLICHTTOOLS_H

#include "EngineTypedefs.h"
#include <string>
#include <sstream>

class IrrlichtTools
{
public:
    static IrrlichtDevicePtr createIrrlichtDevicePtr( irr::IrrlichtDevice *device );

    static irr::gui::IGUIButton *guiCenterFittingButton( irr::gui::IGUIEnvironment *env,
                                                      const std::wstring &label,
                                                       int x, int y );
};

template <typename T, typename ch, typename tr>
std::basic_ostream<ch, tr> &operator<<( std::basic_ostream<ch,tr> &stream,
                          const irr::core::vector3d<T> &in )
{
    stream << in.X << "," << in.Y << "," << in.Z;
    return stream;
}

template <typename T, typename ch, typename tr>
std::basic_istream<ch, tr> &operator>>( std::basic_istream<ch, tr> &stream,
                          irr::core::vector3d<T> &out )
{
    char tmp;
    stream >> out.X >> tmp >> out.Y >> tmp >> out.Z;
    return stream;
}

#endif // IRRLICHTTOOLS_H
