#include "ExploreMenu.h"

ExploreMenu::ExploreMenu( IrrlichtDevicePtr device )
    : mDevice( device )
{
}

ExploreMenu::E_MENU_RESULT ExploreMenu::run()
{
    return EMR_EXIT;
}
