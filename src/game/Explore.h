#ifndef EXPLORE_H
#define EXPLORE_H

#include <engine/EngineTypedefs.h>
#include <engine/LoggerSingleton.h>
#include "GameState.h"

class ExploreMenu;
class ExploreGame;

class EventReceiver;

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
    void initGame();

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;
    PropTreePtr mConfig;

    EventReceiverPtr mEventReceiver;

    bool mRunning;
    E_GAME_STATE mGameState;

    boost::scoped_ptr<ExploreMenu> mMenu;
    boost::scoped_ptr<ExploreGame> mGame;
};

#endif // EXPLORE_H
