#ifndef ITEM_H
#define ITEM_H

#include <engine/EntityContainer.h>
#include <engine/LuaScript.h>
#include "Explore.h"

typedef std::vector<LuaScriptPtr> ScriptVector;

class Item
{
public:
    Item( ExplorePtr explore, PlayerPtr owner, std::string fileName );

    virtual ~Item();

    PropTreePtr getProperties() const;
    EntityPtr getEntity() const;
    PlayerPtr getOwner() const;

    virtual void startAction( int actionID );

    virtual IGUIElementPtr getGUIElement() const;
    virtual ITexturePtr getIcon() const;

protected:
    void create();
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
};

#endif // ITEM_H
