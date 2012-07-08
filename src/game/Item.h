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


#ifndef ITEM_H
#define ITEM_H

#include <engine/EntityContainer.h>
#include <engine/LuaScript.h>
#include "Explore.h"

typedef std::vector<LuaScriptPtr> ScriptVector;
typedef boost::unordered::unordered_map<Entity*,Item*> EntityItemMap;

class Item
{
public:
    Item( ExplorePtr explore, PlayerPtr owner, PropTreePtr properties,
          const std::string &basePath );

    virtual ~Item();

    PropTreePtr getProperties() const;
    EntityContainerPtr getEntities() const;
    PlayerPtr getOwner() const;

    virtual void startAction( int actionID );

    void setGUIVisible( bool visible );

    IGUIElementPtr getGUIElement() const;
    ITexturePtr getIcon() const;

    float getHitPoints() const;
    void setHitPoints( float hitPoints );
    void modifyHitPoints( float difference );

    void setActivationState( bool state );
    bool getActivationState() const;

    static Item* getItemFromEntity( Entity *entity );

protected:
    void registerScripts();
    void registerEntities();
    void loadIcon();

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    BulletWorldPtr mBulletWorld;
    LuaStatePtr mLua;

    PropTreePtr mProperties;
    std::string mBasePath;
    EntityContainerPtr mEntities;

    PlayerPtr mOwner;

    ScriptVector mScripts;

    ITexturePtr mIcon;
    IGUIElementPtr mGUI;

    float mHitPoints;
    bool mActivated;

private:
    static EntityItemMap sEntityItemMap;

    static int sRegisterDummy;
};

#endif // ITEM_H
