#ifndef EXPLOREMENU_H
#define EXPLOREMENU_H

#include <irrlicht/irrlicht.h>
#include <engine/EngineTypedefs.h>

class ExploreMenu
{
public:
    enum E_MENU_RESULT
    {
        EMR_EXIT = 0
    };

    ExploreMenu( IrrlichtDevicePtr device );

    E_MENU_RESULT run();

protected:
    IrrlichtDevicePtr mDevice;
};

#endif // EXPLOREMENU_H
