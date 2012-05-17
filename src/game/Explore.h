#ifndef EXPLORE_H
#define EXPLORE_H

#include <boost/property_tree/ptree.hpp>

#include <engine/EngineTypedefs.h>
#include <engine/LoggerSingleton.h>
#include "GameState.h"
#include "ExploreMenu.h"

class Explore
{
public:
    Explore();
    ~Explore();

    int run();

    template <typename T>
    T readConfigValue( const std::string &key, const T &defaultVal )
    {
        try
        {
            return mConfig->get<T>( key );
        }
        catch( boost::property_tree::ptree_bad_path& )
        {
            _LOG( "Key not found, adding it:", key );
            mConfig->put<T>( key, defaultVal );
            return defaultVal;
        }
    }

private:
    void loadConfig();
    void saveConfig();

    void initIrrlicht();
    void initMenu();

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;
    PropTreePtr mConfig;

    bool mRunning;
    E_GAME_STATE mGameState;

    boost::scoped_ptr<ExploreMenu> mMenu;
};

#endif // EXPLORE_H
