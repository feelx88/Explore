#ifndef EXPLORE_H
#define EXPLORE_H

#include <boost/property_tree/ptree.hpp>

#include <engine/EngineTypedefs.h>
#include "ExploreMenu.h"

class Explore
{
public:
    Explore();

    int run();

private:
    void loadConfig();
    void saveConfig();
    void initIrrlicht();

    IrrlichtDevicePtr mDevice;
    boost::property_tree::ptree mConfig;

    bool mRunning;

    boost::scoped_ptr<ExploreMenu> mMenu;
};

#endif // EXPLORE_H
