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
#include "Explore.h"
#include "IPlayer.h"
#include <engine/PythonScript.h>

typedef boost::unordered::unordered_map<uint8_t, PythonScriptPtr> ScriptMap;
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

    void startAction( uint8_t actionID );

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

    virtual void startActionInternal( uint8_t actionID );

    virtual void serializeInternal( NetworkSyncablePacket &packet,
                                    uint8_t actionID );
    virtual boost::optional<NetworkSyncablePacket> deserializeInternal(
            NetworkSyncablePacket &packet );

    void serializeEntities( NetworkSyncablePacket &packet );
    void deserializeEntities( NetworkSyncablePacket &packet );

    ExplorePtr mExplore;
    IrrlichtDevicePtr mDevice;
    BulletWorldPtr mBulletWorld;

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

    std::string mCacheID;

private:
    static EntityItemMap sEntityItemMap;

    static int sRegisterDummy;
};

#endif // ITEM_H
