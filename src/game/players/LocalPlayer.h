#ifndef LOCALPLAYER_H
#define LOCALPLAYER_H

#include "../IPlayer.h"

class LocalPlayer : public IPlayer
{
public:
    enum E_PLAYER_KEY_MAPPINGS
    {
        EPKM_FORWARD = 0,
        EPKM_BACKWARD,
        EPKM_LEFT,
        EPKM_RIGHT,
        EPKM_JUMP,
        EPKM_MOUSECONTROL,
        EPKM_NEXTSLOT,
        EPKM_PREVIOUSSLOT,
        EPKM_COUNT
    };

    LocalPlayer( ExplorePtr explore );

    ~LocalPlayer();

    void update();

    void switchItem( int index );

    Item *getActiveItem() const;

private:
    void addItems();
    void createGUI();
    void setKeyMappings();
    void processControls();
    void drawCrosshair();

    ICameraSceneNodePtr mCamera;

    irr::EKEY_CODE mKeyMapping[EPKM_COUNT];

    int mActiveItem, mNumItems;

    int mCrossX, mCrossY;
    irr::video::SColor mCrossColor;

    IGUIElementPtr mItemWin;
    irr::gui::IGUIButton *mItemIcons[10];

    bool mJumped;
};

#endif // LOCALPLAYER_H
