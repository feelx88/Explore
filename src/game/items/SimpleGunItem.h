#ifndef SIMPLEGUNITEM_H
#define SIMPLEGUNITEM_H

#include "../Item.h"

class SimpleGunItem : public Item
{
public:
    SimpleGunItem( ExplorePtr explore, PlayerPtr owner );
    ~SimpleGunItem();

    void startAction( int actionID );

    IGUIElementPtr getGUIElement() const;

private:
    void shoot();
    void reload();

    EntityPtr mBullets[10];
    int mCurBullet;

    IGUIElementPtr mGUI;
};

#endif // SIMPLEGUNITEM_H
