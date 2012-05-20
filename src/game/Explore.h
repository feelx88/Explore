#ifndef EXPLORE_H
#define EXPLORE_H

#include <fstream>
#include <boost/enable_shared_from_this.hpp>

#include <engine/EngineTypedefs.h>
#include <engine/LoggerSingleton.h>
#include "GameState.h"

class ExploreMenu;
class ExploreGame;

class EventReceiver;

class Explore : public boost::enable_shared_from_this<Explore>
{
public:
    static ExplorePtr create();

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
            _LOG( "Key not found, adding it", key );
            mConfig->put<T>( key, defaultVal );
            return defaultVal;
        }
    }

    IrrlichtDevicePtr getIrrlichtDevice() const;
    EventReceiverPtr getEventReceiver() const;

private:
    Explore();

    void loadConfig();
    void saveConfig();

    void init();
    void initIrrlicht();
    void initMenu();
    void initGame();

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;

    BulletWorldPtr mBulletWorld;

    PropTreePtr mConfig;

    boost::shared_ptr<std::ofstream> mLogFile;

    EventReceiverPtr mEventReceiver;

    bool mRunning;
    E_GAME_STATE mGameState;

    boost::scoped_ptr<ExploreMenu> mMenu;
    boost::scoped_ptr<ExploreGame> mGame;
};

#endif // EXPLORE_H
