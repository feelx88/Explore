#ifndef GUITOOLS_H
#define GUITOOLS_H

#include "EngineTypedefs.h"
#include <string>

class IrrlichtTools
{
public:
    static IrrlichtDevicePtr createIrrlichtDevicePtr( irr::IrrlichtDevice *device );

    static irr::gui::IGUIButton *guiCenterFittingButton( irr::gui::IGUIEnvironment *env,
                                                      const std::wstring &label,
                                                       int x, int y );
};

#endif // GUITOOLS_H
