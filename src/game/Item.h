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

typedef boost::unordered::unordered_map<E_ITEM_ACTION, LuaScriptPtr> ScriptMap;
typedef boost::unordered::unordered_map<Entity*, Item*> EntityItemMap;

class Item
{
public:
    Item( ExplorePtr explore, IPlayerPtr owner, PropTreePtr properties,
          const std::string &basePath );

    virtual ~Item();

    PropTreePtr getProperties() const;
    EntityContainerPtr getEntities() const;
    IPlayerPtr getOwner() const;

    virtual void startAction( E_ITEM_ACTION actionID );

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

    IPlayerPtr mOwner;

    ScriptMap mScripts;

    ITexturePtr mIcon;
    IGUIElementPtr mGUI;

    float mHitPoints;
    bool mActivated;
    bool mDestructible;

private:
    static EntityItemMap sEntityItemMap;

    static int sRegisterDummy;
};

static std::ostream &operator<<( std::ostream &stream,
                          const E_ITEM_ACTION &in )
{
    if( in == EIA_FIRST_ACTION )
        stream << "FIRST";
    else if( in == EIA_SECOND_ACTION )
        stream << "SECOND";
    else if( in == EIA_USE_ACTION )
        stream << "USE";
    else if( in == EIA_UPDATE_ACTION )
        stream << "UPDATE";
    else if( in == EIA_DESTROY_ACTION )
        stream << "DESTROY";
    else
        stream << "UNDEFINED";

    return stream;
}

static std::istream &operator>>( std::istream &stream,
                          E_ITEM_ACTION &out )
{
    std::string action;
    stream >> action;

    if( action == "FIRST" )
        out = EIA_FIRST_ACTION;
    else if( action == "SECOND" )
        out = EIA_SECOND_ACTION;
    else if( action == "USE" )
        out = EIA_USE_ACTION;
    else if( action == "UPDATE" )
        out = EIA_UPDATE_ACTION;
    else if( action == "DESTROY" )
        out = EIA_DESTROY_ACTION;
    else
        out = EIA_COUNT;

    return stream;
}

#endif // ITEM_H
