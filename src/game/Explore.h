#ifndef EXPLORE_H
#define EXPLORE_H

#include <fstream>
#include <boost/enable_shared_from_this.hpp>

#include <engine/EngineTypedefs.h>
#include <engine/EventReceiver.h>
#include <engine/LoggerSingleton.h>
#include "GameState.h"

class Player;
class Item;

typedef Player* PlayerPtr;
typedef boost::shared_ptr<Item> ItemPtr;

class ExploreMenu;
class ExploreGame;

class EventReceiver;

class BulletIrrlichtDebugDrawer : public btIDebugDraw
{
public:
    irr::video::IVideoDriver *driver;
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        driver->setMaterial( driver->getMaterial2D() );
        driver->setTransform( irr::video::ETS_VIEW, irr::core::IdentityMatrix );
        irr::video::SColor c( 255,
                                  (int)color.x() * 255, (int)color.y() * 255,
                                  (int)color.z() * 255 );
        driver->draw3DLine( irr::core::vector3df( from.x(), from.y(), from.z() ),
                            irr::core::vector3df( to.x(), to.y(), to.z() ),
                            c );
    }

    virtual void	drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color)
    {
    }

    virtual void	reportErrorWarning(const char* warningString)
    {
    }

    virtual void	draw3dText(const btVector3& location,const char* textString)
    {
    }

    virtual void	setDebugMode(int debugMode)
    {
    }

    virtual int		getDebugMode() const
    {
        return DBG_MAX_DEBUG_DRAW_MODE;
    }
};


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
            _LOG( "Key not found, adding it", key );
            mConfig->put<T>( key, defaultVal );
            return defaultVal;
        }
    }

    void setGameState( const E_GAME_STATE &state );

    IrrlichtDevicePtr getIrrlichtDevice() const;
    EventReceiverPtr getEventReceiver() const;
    BulletWorldPtr getBulletWorld() const;
    LuaStatePtr getLuaVM() const;

private:

    void loadConfig();
    void saveConfig();

    void initIrrlicht();
    void initBullet();
    void initLua();
    void initMenu();
    void initGame();

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;

    BulletWorldPtr mBulletWorld;
    CollisionConfigurationPtr mCollisionConfiguration;
    DispatcherPtr mDispatcher;
    BroadphasePtr mBroadphase;
    ConstraintSolverPtr mConstraintSolver;

    LuaStatePtr mLua;

    PropTreePtr mConfig;

    boost::shared_ptr<std::ofstream> mLogFile;

    EventReceiverPtr mEventReceiver;

    bool mRunning;
    E_GAME_STATE mGameState;

    boost::scoped_ptr<ExploreMenu> mMenu;
    boost::scoped_ptr<ExploreGame> mGame;

    boost::scoped_ptr<BulletIrrlichtDebugDrawer> mBulletDebugDrawer;
};

#endif // EXPLORE_H
