/*
    Copyright 2012 Felix Müller.

    This file is part of Explore.

    Explore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Explore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Explore.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef EXPLORE_H
#define EXPLORE_H

#include <engine/EngineTypedefs.h>

#include <fstream>
#include <boost/enable_shared_from_this.hpp>

#include <engine/EventReceiver.h>
#include <engine/LoggerSingleton.h>
#include <engine/ScriptConsole.h>
#include <GameState.h>

class Item;

typedef boost::shared_ptr<Item> ItemPtr;

class ExploreMenu;
class ExploreGame;

#include <network/ExploreServer.h>
typedef boost::shared_ptr<ExploreMenu> ExploreMenuPtr;
typedef boost::shared_ptr<ExploreGame> ExploreGamePtr;
typedef boost::shared_ptr<ExploreServer> ExploreServerPtr;

class EventReceiver;

class APIEXPORT BulletIrrlichtDebugDrawer : public btIDebugDraw
{
public:
    irr::video::IVideoDriver *driver;
    void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
    {
        driver->setMaterial( driver->getMaterial2D() );
        irr::video::SColor c( 255,
                                  (int)color.x() * 255, (int)color.y() * 255,
                                  (int)color.z() * 255 );
        driver->draw3DLine( irr::core::vector3df( from.x(), from.y(), from.z() ),
                            irr::core::vector3df( to.x(), to.y(), to.z() ),
                            c );
    }

    virtual void	drawContactPoint(const btVector3& /*PointOnB*/,
                                     const btVector3& /*normalOnB*/,
                                     btScalar /*distance*/,
                                     int /*lifeTime*/,
                                     const btVector3& /*color*/)
    {
    }

    virtual void	reportErrorWarning(const char* /*warningString*/)
    {
    }

    virtual void	draw3dText(const btVector3& /*location*/,
                               const char* /*textString*/)
    {
    }

    virtual void	setDebugMode(int /*debugMode*/)
    {
    }

    virtual int		getDebugMode() const
    {
        return DBG_MAX_DEBUG_DRAW_MODE;
    }
};


class APIEXPORT Explore
{
public:

    Explore( StringVector args );
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

    template <typename T>
    void writeConfigValue( const std::string &key, const T &value )
    {
        mConfig->put<T>( key, value );
    }

    void setGameState( const E_GAME_STATE &state );
    E_GAME_STATE getGameState() const;

    IrrlichtDevicePtr getIrrlichtDevice() const;
    EventReceiverPtr getEventReceiver() const;
    BulletWorldPtr getBulletWorld() const;
    ScriptConsolePtr getScriptConsole() const;
    IOServicePtr getIOService() const;

    ExploreMenuPtr getExploreMenu();
    ExploreGamePtr getExploreGame();
    ExploreServerPtr getExploreServer();

    const StringVector &getAvailableItems() const;

    static irr::EKEY_CODE getKeyCode( const std::string &name );

    void saveConfig();

private:

    void parseCommandLine();
    void loadConfig();

    void initIrrlicht();
    void initBullet();
    void initScriptConsole();
    void initMenu();
    void initGame();
    void initServer();

    StringVector mArgs;

    IrrlichtDevicePtr mDevice;
    IVideoDriverPtr mVideoDriver;
    IGUIEnvironmentPtr mGUI;

    BulletWorldPtr mBulletWorld;
    CollisionConfigurationPtr mCollisionConfiguration;
    DispatcherPtr mDispatcher;
    BroadphasePtr mBroadphase;
    ConstraintSolverPtr mConstraintSolver;
    GhostPairCallback mGhostPairCallback;

    std::string mConfigFileName;
    PropTreePtr mConfig;
    bool mDedicated;

    boost::shared_ptr<std::ofstream> mLogFile;

    EventReceiverPtr mEventReceiver;

    bool mRunning;
    E_GAME_STATE mGameState;

    ExploreMenuPtr mMenu;
    ExploreGamePtr mGame;
    ExploreServerPtr mServer;
    NetworkMessengerPtr mMessenger;

    boost::scoped_ptr<BulletIrrlichtDebugDrawer> mBulletDebugDrawer;

    static PropTreePtr sKeyCodes;

    ScriptConsolePtr mScriptConsole;

    StringVector mAvailableItems;

    IOServicePtr mIOService;
};

#endif // EXPLORE_H
