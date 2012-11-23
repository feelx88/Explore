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
#include "IPlayer.h"

typedef boost::unordered::unordered_map<uint8_t, LuaScriptPtr> ScriptMap;
typedef boost::unordered::unordered_map<Entity*, Item*> EntityItemMap;

class Item : public NetworkSyncable
{
public:
    template <class T>
    friend class ItemCreator;
    friend class ItemFactory;

    virtual ~Item();

    PropTreePtr getProperties() const;
    EntityContainerPtr getEntities() const;
    virtual void setOwner( IPlayerPtr owner );
    IPlayerPtr getOwner() const;

    virtual void startAction( uint8_t actionID );

    void setGUIVisible( bool visible );

    IGUIElementPtr getGUIElement() const;
    ITexturePtr getIcon() const;

    float getHitPoints() const;
    void setHitPoints( float hitPoints );
    void modifyHitPoints( float difference );

    void setActivationState( bool state );
    bool getActivationState() const;

    std::string getFileName() const;

    static Item* getItemFromEntity( Entity *entity );

protected:

    Item( ExplorePtr explore, IPlayerPtr owner, PropTreePtr properties,
          const std::string &basePath );

    void registerScripts();
    void registerEntities();
    void loadIcon();

    virtual void serializeInternal( NetworkSyncablePacket &packet,
                                    uint8_t actionID );
    virtual boost::optional<NetworkSyncablePacket> deserializeInternal(
            NetworkSyncablePacket &packet );

    void serializeEntities( NetworkSyncablePacket &packet );
    void deserializeEntities( NetworkSyncablePacket &packet );

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    BulletWorldPtr mBulletWorld;
    LuaStatePtr mLua;

    PropTreePtr mProperties;
    std::string mBasePath;
    EntityContainerPtr mEntities;

    IPlayerWeakPtr mOwner;

    ScriptMap mScripts;

    ITexturePtr mIcon;
    IGUIElementPtr mGUI;

    float mHitPoints;
    bool mActivated;
    bool mDestructible;

    std::string mFileName;

private:
    static EntityItemMap sEntityItemMap;

    static int sRegisterDummy;
};

/*static inline std::ostream &operator<<( std::ostream &stream,
                          const uint8_t &in )
{
    if( in == EIAID_FIRST_ACTION )
        stream << "FIRST";
    else if( in == EIAID_SECOND_ACTION )
        stream << "SECOND";
    else if( in == EIAID_USE_ACTION )
        stream << "USE";
    else if( in == EAID_UPDATE )
        stream << "UPDATE";
    else if( in == EAID_DESTROY )
        stream << "DESTROY";
    else
        stream << "UNDEFINED";

    return stream;
}

static inline std::istream &operator>>( std::istream &stream,
                          uint8_t &out )
{
    std::string action;
    stream >> action;

    if( action == "FIRST" )
        out = EIAID_FIRST_ACTION;
    else if( action == "SECOND" )
        out = EIAID_SECOND_ACTION;
    else if( action == "USE" )
        out = EIAID_USE_ACTION;
    else if( action == "UPDATE" )
        out = EAID_UPDATE;
    else if( action == "DESTROY" )
        out = EAID_DESTROY;
    else
        out = EAID_COUNT;

    return stream;
}*/

#endif // ITEM_H
