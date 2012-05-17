#ifndef EXPLOREMENU_H
#define EXPLOREMENU_H

#include <irrlicht/irrlicht.h>
#include <engine/EngineTypedefs.h>

class ExploreMenu
{
public:
    ExploreMenu( IrrlichtDevicePtr device );

    void run();

protected:
    IrrlichtDevicePtr mDevice;

};

#endif // EXPLOREMENU_H
